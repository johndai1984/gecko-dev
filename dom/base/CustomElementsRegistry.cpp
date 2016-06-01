/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/dom/CustomElementsRegistry.h"
#include "mozilla/dom/CustomElementsRegistryBinding.h"

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

void CustomElementsRegistry::Define(JSContext* aCx, const nsAString& aName,
                                    Function& aFunctionConstructor,
                                    const ElementDefinitionOptions& aOptions,
                                    JS::MutableHandle<JSObject*> aRetVal,
                                    ErrorResult& aRv)
{
  //Element definition process
  //which is a process of adding a custom element definition to the CustomElementsRegistry.
  // Implements
  //https://html.spec.whatwg.org/multipage/scripting.html#element-definition
  //step 1
  JS::Handle<JSObject*> fctor = aFunctionConstructor.Callable();
  if (!JS::IsConstructor(fctor)) {
    aRv.Throw(NS_ERROR_TYPE_ERR);
    return;
  }

  //step 2
  // Unconditionally convert name to lowercase.
  nsAutoString lcType;
  nsContentUtils::ASCIIToLower(aName, lcType);
  nsCOMPtr<nsIAtom> typeAtom(NS_Atomize(lcType));
  if (!nsContentUtils::IsCustomElementName(typeAtom)) {
    aRv.Throw(NS_ERROR_DOM_SYNTAX_ERR);
    return;
  }

  //step 3
  // If there already exists a definition with the same TYPE, set ERROR to
  // DuplicateDefinition and stop.
  // Note that we need to find existing custom elements from either namespace.
  CustomElementHashKey duplicateFinder(kNameSpaceID_Unknown, typeAtom);
  if (mCustomDefinitions.Get(&duplicateFinder)) {
    aRv.Throw(NS_ERROR_DOM_NOT_SUPPORTED_ERR);
    return;
  }

  //step 4
  if(mConstructors.Contains(&aFunctionConstructor)) {
    aRv.Throw(NS_ERROR_DOM_NOT_SUPPORTED_ERR);
    return;
  }

  //step 5
  nsAutoString lcName;
  nsContentUtils::ASCIIToLower(aName, lcName);

  //step 6
  nsString extends;
  if (aOptions.mExtends.WasPassed()) {
    extends.Assign(aOptions.mExtends.Value());
  }

  //step 7
  if (extends.IsVoid()) {
    //step 7.1
    nsCOMPtr<nsIAtom> extendsAtom(NS_Atomize(extends));
    if (nsContentUtils::IsCustomElementName(extendsAtom)) {
      aRv.Throw(NS_ERROR_DOM_NOT_SUPPORTED_ERR);
      return;
    }
    //step 7.2
    // Element* element;
    // RefPtr<mozilla::dom::NodeInfo> ni;
    // nsHTMLDocument* htmlDoc = mDoc->AsHTMLDocument();
    // if (htmlDoc) {
    //   element = htmlDoc->GetBody();
    // } else {
    //   element = mDoc->GetDocumentElement();
    // }
    // if (element) {
    //   ni = element->GetExistingAttrNameFromQName(extends);
    // }

    //step 7.3
    lcName.Assign(extends);
  }
  //step 8, 9

  //step 10
  JS::Rooted<JS::Value> protoValue(aCx);
  if(!JS_GetProperty(aCx, fctor, "prototype", &protoValue)) {
    aRv.Throw(NS_ERROR_UNEXPECTED);
    return;
  }

  //step 11
  if (JS_TypeOfValue(aCx, protoValue) != JSTYPE_OBJECT) {
    aRv.Throw(NS_ERROR_TYPE_ERR);
    return;
  }

  JS::Rooted<JSObject*> protoObject(aCx);
  if (!JS_ValueToObject(aCx, protoValue, &protoObject)) {
    aRv.Throw(NS_ERROR_UNEXPECTED);
    return;
  }


  //step 12-17

  //step 18
  // Associate the definition with the custom element.
  nsCOMPtr<nsIAtom> localNameAtom = NS_Atomize(lcName);
  CustomElementHashKey key(kNameSpaceID_XHTML, typeAtom);
  CustomElementDefinition* definition =
    new CustomElementDefinition(typeAtom,
                                localNameAtom,
                                aFunctionConstructor,
                                protoObject);

  mCustomDefinitions.Put(&key, definition);
  mConstructors.PutEntry(&aFunctionConstructor);
}

void
CustomElementsRegistry::Get(JSContext* aCx, const nsAString& aName,
                            JS::MutableHandle<JS::Value> aRetVal,
                            ErrorResult& aRv)
{
//Retrieves the custom element constructor defined for the given name.
//Returns undefined if there is no custom element definition with the given name.

  nsAutoString lcType;
  nsContentUtils::ASCIIToLower(aName, lcType);
  nsCOMPtr<nsIAtom> nameAtom(NS_Atomize(lcType));
  if (!nameAtom) {
    aRetVal.setNull();
    return;
  }
  CustomElementDefinition* data;
  CustomElementHashKey key(kNameSpaceID_Unknown, nameAtom);
  if (!mCustomDefinitions.Get(&key, &data)) {
    aRetVal.setNull();
    return;
  }

  JSAutoCompartment ac(aCx, GetWrapper());
  JS::Handle<JSObject*> fctor = (data->mConstructor)->Callable();
  aRetVal.setObject(*fctor);
  // aRv = nsContentUtils::XPConnect()->VariantToJS(aCx, GetWrapper(),
  //                                                data->mConstructor,
  //                                                aRetval);
}

already_AddRefed<Promise>
CustomElementsRegistry::WhenDefined(const nsAString& name, ErrorResult& aRv)
{
  // //  nsCOMPtr<nsIGlobalObject> global = do_QueryInterface(mWindow);
  //   nsCOMPtr<nsIGlobalObject> global = nullptr;
  //   if (!global) {
  //     aRv.Throw(NS_ERROR_FAILURE);
  //     return nullptr;
  //   }
  //   return Promise::Create(global, aRv);
  RefPtr<Promise> promise = nullptr;//CreatePromise(aRv);
  if (aRv.Failed()) {
    return nullptr;
  }
  //Returns a promise that will be fulfilled
  //when a custom element becomes defined with the given name.
  //(If such a custom element is already defined,
  //the returned promise will be immediately fulfilled.)
  //Returns a promise rejected with a "SyntaxError" DOMException
  //if not given a valid custom element name.
  return promise.forget();
}

} // namespace dom
} // namespace mozilla

namespace mozilla {
namespace dom {
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

CustomElementDefinition::CustomElementDefinition(nsIAtom* aType,
                                                 nsIAtom* aLocalName,
                                                 mozilla::dom::Function& aConstructor,
                                                 JSObject* aPrototype)
: mType(aType),
  mLocalName(aLocalName),
  mConstructor(&aConstructor),
  mPrototype(aPrototype)
{
}



} // namespace dom
} // namespace mozilla