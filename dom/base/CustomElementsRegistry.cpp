/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/dom/CustomElementsRegistry.h"
#include "mozilla/dom/CustomElementsRegistryBinding.h"
#include "mozilla/dom/HTMLElementBinding.h"
#include "mozilla/dom/WebComponentsBinding.h"

namespace mozilla {
namespace dom {


// Only needed for refcounted objects.
NS_IMPL_CYCLE_COLLECTION_WRAPPERCACHE(CustomElementsRegistry, mWindow)
NS_IMPL_CYCLE_COLLECTING_ADDREF(CustomElementsRegistry)
NS_IMPL_CYCLE_COLLECTING_RELEASE(CustomElementsRegistry)
NS_INTERFACE_MAP_BEGIN_CYCLE_COLLECTION(CustomElementsRegistry)
  NS_WRAPPERCACHE_INTERFACE_MAP_ENTRY
  NS_INTERFACE_MAP_ENTRY(nsISupports)
NS_INTERFACE_MAP_END

/* static */ bool
CustomElementsRegistry::IsCustomElementEnabled(JSContext* aCx, JSObject* aObject)
{
  if (Preferences::GetBool("dom.webcomponents.customelement.enabled") ||
      nsDocument::IsWebComponentsEnabled(aCx, aObject)) {
    return true;
  }

  return false;
}

/* static */ already_AddRefed<CustomElementsRegistry>
CustomElementsRegistry::Create(nsPIDOMWindowInner* aWindow)
{
  MOZ_ASSERT(aWindow);
  MOZ_ASSERT(aWindow->IsInnerWindow());

  if (!aWindow->GetDocShell()) {
    return nullptr;
  }

  RefPtr<CustomElementsRegistry> customElementsRegistry =
    new CustomElementsRegistry(aWindow);
  return customElementsRegistry.forget();
}

CustomElementsRegistry::CustomElementsRegistry(nsPIDOMWindowInner* aWindow)
 : mWindow(aWindow)
{
}

CustomElementsRegistry::~CustomElementsRegistry()
{
}

JSObject*
CustomElementsRegistry::WrapObject(JSContext* aCx, JS::Handle<JSObject*> aGivenProto)
{
  return CustomElementsRegistryBinding::Wrap(aCx, this, aGivenProto);
}

nsISupports* CustomElementsRegistry::GetParentObject() const
{
  return mWindow;
}

void CustomElementsRegistry::Define(const nsAString& aName,
                                    Function& aFunctionConstructor,
                                    const ElementDefinitionOptions& aOptions,
                                    ErrorResult& aRv)
{
  // https://html.spec.whatwg.org/multipage/scripting.html#element-definition

  AutoJSAPI jsapi;
  if (NS_WARN_IF(!jsapi.Init(mWindow))) {
    aRv.Throw(NS_ERROR_FAILURE);
    return;
  }

  JSContext *cx = jsapi.cx();

  JS::Rooted<JSObject*> constructor(cx, aFunctionConstructor.Callable());
  JS::Rooted<JSObject*> constructorUnwrapped(cx, js::CheckedUnwrap(constructor));
  if (!constructorUnwrapped) {
    // If the caller's compartment does not have permission to access the
    // unwrapped prototype then throw.
    aRv.Throw(NS_ERROR_DOM_SECURITY_ERR);
    return;
  }

  if (!JS::IsConstructor(constructorUnwrapped)) {
    aRv.ThrowTypeError<MSG_NOT_CONSTRUCTOR>(NS_LITERAL_STRING("Argument 2 of CustomElementsRegistry.define"));
    return;
  }

  JS::Rooted<JSObject*> constructorPrototype(cx);
  // Enter constructorUnwrapped's compartment.
  {
    JSAutoCompartment ac(cx, constructorUnwrapped);

    // Interface object whose corresponding interface either is HTMLElement or
    // its subclass is not allowed.
    const js::Class* clasp = js::GetObjectClass(constructorUnwrapped);
    if (IsDOMIfaceAndProtoClass(clasp)) {
      JS::Rooted<JSObject*> htmlInterface(cx);
      htmlInterface = HTMLElementBinding::GetConstructorObject(cx);

      JS::Rooted<JSObject*> constructorProto(cx, constructorUnwrapped);
      while (constructorProto) {
        if (constructorProto == htmlInterface) {
          aRv.ThrowTypeError<MSG_ILLEGAL_CONSTRUCTOR>();
          return;
        }

        if (!JS_GetPrototype(cx, constructorProto, &constructorProto)) {
          JS_ClearPendingException(cx);
          aRv.Throw(NS_ERROR_UNEXPECTED);
          return;
        }
      }
    }

    // Named constructor whose corresponding interface either is HTMLElement or
    // its subclass is not allowed.
    JS::Rooted<JS::Value> prototypev(cx);
    if (!JS_GetProperty(cx, constructorUnwrapped, "prototype", &prototypev)) {
      JS_ClearPendingException(cx);
      aRv.Throw(NS_ERROR_UNEXPECTED);
      return;
    }

    if (!prototypev.isObject()) {
      aRv.ThrowTypeError<MSG_NOT_OBJECT>(NS_LITERAL_STRING("constructor.prototype"));
      return;
    }

    constructorPrototype = &prototypev.toObject();
    if (IsDOMIfaceAndProtoClass(js::GetObjectClass(constructorPrototype))) {
      JS::Rooted<JSObject*> htmlProto(cx);
      htmlProto = HTMLElementBinding::GetProtoObjectHandle(cx);

      JS::Rooted<JSObject*> prototype(cx, constructorPrototype);
      while (prototype) {
        if (prototype == htmlProto) {
          aRv.ThrowTypeError<MSG_ILLEGAL_CONSTRUCTOR>();
          return;
        }

        if (!JS_GetPrototype(cx, prototype, &prototype)) {
          JS_ClearPendingException(cx);
          aRv.Throw(NS_ERROR_UNEXPECTED);
          return;
        }
      }
    }
  } // Leave constructorUnwrapped's compartment.

  // Check if name is a valid custom element name.
  // TODO: Bug 1275832 -  Implement custom element name validation for custom element.
  nsCOMPtr<nsIAtom> nameAtom(NS_Atomize(aName));
  if (!nsContentUtils::IsCustomElementName(nameAtom)) {
    aRv.Throw(NS_ERROR_DOM_SYNTAX_ERR);
    return;
  }

  // TODO: Another part: move mRegisry from nsDocument to here.
  /**
   * 3. If name is not a valid custom element name, then throw a "SyntaxError" DOMException and abort these steps.
   * 4. If this CustomElementsRegistry contains an entry with name name, then throw a "NotSupportedError" DOMException and abort these steps.
   * 5. If this CustomElementsRegistry's set of being-defined names contains name, then throw a "NotSupportedError" DOMException and abort these steps.
   * 6. If this CustomElementsRegistry contains an entry with constructor constructor, then throw a "NotSupportedError" DOMException and abort these steps.
   * 7. If this CustomElementsRegistry's set of being-defined constructors contains constructor, then throw a "NotSupportedError" DOMException and abort these steps.
   */

  nsAutoString localName(aName);
  if (aOptions.mExtends.WasPassed()) {
    nsCOMPtr<nsIAtom> extendsAtom(NS_Atomize(aOptions.mExtends.Value()));
    // TODO: Bug 1275832 -  Implement custom element name validation for custom element.
    if (nsContentUtils::IsCustomElementName(nameAtom)) {
      aRv.Throw(NS_ERROR_DOM_SYNTAX_ERR);
      return;
    }

    // TODO: 10.2. if extends does not indicate an element definition in this specification.
    // https://html.spec.whatwg.org/multipage/dom.html#htmlunknownelement
    //if () {
    //  aRv.Throw(NS_ERROR_DOM_NOT_SUPPORTED_ERR);
    //  return;
    //}

    localName.Assign(aOptions.mExtends.Value());
  }

  // TODO: Another part: move mRegisry from nsDocument to here.
  /**
   * 11. Add name to this CustomElementsRegistry's set of being-defined names.
   * 12. Add constructor to this CustomElementsRegistry's set of being-defined constructors.
   */

  // Enter constructorPrototype's compartment
  {
    JSAutoCompartment ac(cx, constructorPrototype);

    JS::Rooted<JSObject*> connectedCallback(cx);
    JS::Rooted<JSObject*> disconnectedCallback(cx);
    JS::Rooted<JSObject*> attributeChangedCallback(cx);

    JS::Rooted<JS::Value> connectedCallbackValue(cx);
    if (!JS_GetProperty(cx, constructorPrototype, "connectedCallback", &connectedCallbackValue)) {
      JS_ClearPendingException(cx);
      aRv.Throw(NS_ERROR_UNEXPECTED);
      return;
    }

    // TODO: 13.5. If connectedCallbackValue is not undefined, then set connectedCallback
    //             to the result of converting connectedCallbackValue to the Web IDL Function
    //             callback type. Rethrow any exceptions.

    JS::Rooted<JS::Value> disconnectedCallbackValue(cx);
    if (!JS_GetProperty(cx, constructorPrototype, "disconnectedCallback", &disconnectedCallbackValue)) {
      JS_ClearPendingException(cx);
      aRv.Throw(NS_ERROR_UNEXPECTED);
      return;
    }

    // TODO: 13.7. If disconnectedCallbackValue is not undefined, then set disconnectedCallback
    //             to the result of converting disconnectedCallbackValue to the Web IDL Function
    //             callback type. Rethrow any exceptions.

    JS::Rooted<JS::Value> attributeChangedCallbackValue(cx);
    if (!JS_GetProperty(cx, constructorPrototype, "attributeChangedCallback", &attributeChangedCallbackValue)) {
      JS_ClearPendingException(cx);
      aRv.Throw(NS_ERROR_UNEXPECTED);
      return;
    }

    // TODO: 13.9. If attributeChangedCallbackValue is not undefined, then set attributeChangedCallback
    //             to the result of converting attributeChangedCallbackValue to the Web IDL Function
    //             callback type. Rethrow any exceptions.

    // TODO: 13.10, 13.11, substeps 1, 2.
  } // Leave constructorPrototype's compartment.

  // TODO: Another part: move mRegisry from nsDocument to here.
  // 14, 15, 16

  // TODO: upgrade all elements.

  // TODO: when-defined promise.
}

void
CustomElementsRegistry::Get(JSContext* aCx, const nsAString& aName,
                            JS::MutableHandle<JS::Value> aRetVal,
                            ErrorResult& aRv)
{
  // TODO: This function will be implemented in bug 1275838
  aRv.Throw(NS_ERROR_NOT_IMPLEMENTED);
}

already_AddRefed<Promise>
CustomElementsRegistry::WhenDefined(const nsAString& name, ErrorResult& aRv)
{
  // TODO: This function will be implemented in bug 1275839
  aRv.Throw(NS_ERROR_NOT_IMPLEMENTED);
  return nullptr;
}

CustomElementDefinition::CustomElementDefinition(JSObject* aPrototype,
                                                 nsIAtom* aType,
                                                 nsIAtom* aLocalName,
                                                 LifecycleCallbacks* aCallbacks,
                                                 uint32_t aNamespaceID,
                                                 uint32_t aDocOrder)
  : mPrototype(aPrototype),
    mType(aType),
    mLocalName(aLocalName),
    mCallbacks(aCallbacks),
    mNamespaceID(aNamespaceID),
    mDocOrder(aDocOrder)
{
}

} // namespace dom
} // namespace mozilla