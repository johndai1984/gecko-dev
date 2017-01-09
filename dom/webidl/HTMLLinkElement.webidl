/* -*- Mode: IDL; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The origin of this IDL file is
 * http://www.whatwg.org/specs/web-apps/current-work/#the-link-element
 * http://www.whatwg.org/specs/web-apps/current-work/#other-elements,-attributes-and-apis
 * © Copyright 2004-2011 Apple Computer, Inc., Mozilla Foundation, and
 * Opera Software ASA. You are granted a license to use, reproduce
 * and create derivative works of this document.
 */

// http://www.whatwg.org/specs/web-apps/current-work/#the-link-element
[HTMLConstructor]
interface HTMLLinkElement : HTMLElement {
  [Pure]
           attribute boolean disabled;
  [SetterThrows, Pure, CEReactions]
           attribute DOMString href;
  [SetterThrows, Pure, CEReactions]
           attribute DOMString? crossOrigin;
  [SetterThrows, Pure, CEReactions]
           attribute DOMString rel;
  [PutForwards=value, CEReactions]
  readonly attribute DOMTokenList relList;
  [SetterThrows, Pure, CEReactions]
           attribute DOMString media;
  [SetterThrows, Pure, CEReactions]
           attribute DOMString hreflang;
  [SetterThrows, Pure, CEReactions]
           attribute DOMString type;
  [SetterThrows, Pure, Pref="network.http.enablePerElementReferrer", CEReactions]
           attribute DOMString referrerPolicy;
  [PutForwards=value, CEReactions] readonly attribute DOMTokenList sizes;
};
HTMLLinkElement implements LinkStyle;

// http://www.whatwg.org/specs/web-apps/current-work/#other-elements,-attributes-and-apis
partial interface HTMLLinkElement {
  [SetterThrows, Pure, CEReactions]
           attribute DOMString charset;
  [SetterThrows, Pure, CEReactions]
           attribute DOMString rev;
  [SetterThrows, Pure, CEReactions]
           attribute DOMString target;
};

// http://w3c.github.io/webcomponents/spec/imports/#interface-import
partial interface HTMLLinkElement {
    [Func="nsDocument::IsWebComponentsEnabled"]
    readonly attribute Document? import;
};

// https://w3c.github.io/webappsec/specs/subresourceintegrity/#htmllinkelement-1
partial interface HTMLLinkElement {
  [SetterThrows]
  attribute DOMString integrity;
};
