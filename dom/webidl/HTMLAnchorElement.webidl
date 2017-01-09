/* -*- Mode: IDL; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The origin of this IDL file is
 * http://www.whatwg.org/specs/web-apps/current-work/#the-a-element
 * http://www.whatwg.org/specs/web-apps/current-work/#other-elements,-attributes-and-apis
 * © Copyright 2004-2011 Apple Computer, Inc., Mozilla Foundation, and
 * Opera Software ASA. You are granted a license to use, reproduce
 * and create derivative works of this document.
 */

// http://www.whatwg.org/specs/web-apps/current-work/#the-a-element
[HTMLConstructor]
interface HTMLAnchorElement : HTMLElement {
           [SetterThrows, CEReactions]
           attribute DOMString target;
           [SetterThrows, CEReactions]
           attribute DOMString download;
           [SetterThrows, CEReactions]
           attribute DOMString ping;
           [SetterThrows, CEReactions]
           attribute DOMString rel;
           [SetterThrows, Pref="network.http.enablePerElementReferrer", CEReactions]
           attribute DOMString referrerPolicy;
           [PutForwards=value, CEReactions]
  readonly attribute DOMTokenList relList;
           [SetterThrows, CEReactions]
           attribute DOMString hreflang;
           [SetterThrows, CEReactions]
           attribute DOMString type;

           [SetterThrows, CEReactions]
           attribute DOMString text;
};

HTMLAnchorElement implements HTMLHyperlinkElementUtils;

// http://www.whatwg.org/specs/web-apps/current-work/#other-elements,-attributes-and-apis
partial interface HTMLAnchorElement {
           [SetterThrows, CEReactions]
           attribute DOMString coords;
           [SetterThrows, CEReactions]
           attribute DOMString charset;
           [SetterThrows, CEReactions]
           attribute DOMString name;
           [SetterThrows, CEReactions]
           attribute DOMString rev;
           [SetterThrows, CEReactions]
           attribute DOMString shape;
};
