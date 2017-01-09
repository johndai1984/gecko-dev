/* -*- Mode: IDL; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The origin of this IDL file is
 * http://www.whatwg.org/specs/web-apps/current-work/#the-textarea-element
 * http://www.whatwg.org/specs/web-apps/current-work/#other-elements,-attributes-and-apis
 * © Copyright 2004-2011 Apple Computer, Inc., Mozilla Foundation, and
 * Opera Software ASA. You are granted a license to use, reproduce
 * and create derivative works of this document.
 */

interface nsIEditor;
interface MozControllers;

[HTMLConstructor]
interface HTMLTextAreaElement : HTMLElement {
           // attribute DOMString autocomplete;
  [SetterThrows, Pure, CEReactions]
           attribute boolean autofocus;
  [SetterThrows, Pure, CEReactions]
           attribute unsigned long cols;
           // attribute DOMString dirName;
  [SetterThrows, Pure, CEReactions]
           attribute boolean disabled;
  [Pure]
  readonly attribute HTMLFormElement? form;
           // attribute DOMString inputMode;
  [SetterThrows, Pure, CEReactions]
           attribute long maxLength;
  [SetterThrows, Pure, CEReactions]
           attribute long minLength;
  [SetterThrows, Pure, CEReactions]
           attribute DOMString name;
  [SetterThrows, Pure, CEReactions]
           attribute DOMString placeholder;
  [SetterThrows, Pure, CEReactions]
           attribute boolean readOnly;
  [SetterThrows, Pure, CEReactions]
           attribute boolean required;
  [SetterThrows, Pure, CEReactions]
           attribute unsigned long rows;
  [SetterThrows, Pure, CEReactions]
           attribute DOMString wrap;

  [Constant]
  readonly attribute DOMString type;
  [SetterThrows, Pure, CEReactions]
           attribute DOMString defaultValue;
  [TreatNullAs=EmptyString, CEReactions] attribute DOMString value;
  readonly attribute unsigned long textLength;

  readonly attribute boolean willValidate;
  readonly attribute ValidityState validity;
  readonly attribute DOMString validationMessage;
  boolean checkValidity();
  boolean reportValidity();
  void setCustomValidity(DOMString error);

  // readonly attribute NodeList labels;

  void select();
  [Throws]
           attribute unsigned long? selectionStart;
  [Throws]
           attribute unsigned long? selectionEnd;
  [Throws]
           attribute DOMString? selectionDirection;
  [Throws]
  void setRangeText(DOMString replacement);
  [Throws]
  void setRangeText(DOMString replacement, unsigned long start,
    unsigned long end, optional SelectionMode selectionMode = "preserve");
  [Throws]
  void setSelectionRange(unsigned long start, unsigned long end, optional DOMString direction);
};

partial interface HTMLTextAreaElement {
  // Mirrored chrome-only Mozilla extensions to nsIDOMHTMLTextAreaElement.
  // Please make sure to update this list of nsIDOMHTMLTextAreaElement changes.

  [Throws, ChromeOnly]
  readonly attribute MozControllers controllers;
};

partial interface HTMLTextAreaElement {
  // Mirrored chrome-only nsIDOMNSEditableElement methods.  Please make sure
  // to update this list if nsIDOMNSEditableElement changes.

  [ChromeOnly]
  readonly attribute nsIEditor? editor;

  // This is similar to set .value on nsIDOMInput/TextAreaElements, but
  // handling of the value change is closer to the normal user input, so
  // 'change' event for example will be dispatched when focusing out the
  // element.
  [ChromeOnly]
  void setUserInput(DOMString input);
};
