/* -*- Mode: IDL; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The origin of this IDL file is
 * http://www.whatwg.org/specs/web-apps/current-work/#the-iframe-element
 * http://www.whatwg.org/specs/web-apps/current-work/#other-elements,-attributes-and-apis
 * © Copyright 2004-2011 Apple Computer, Inc., Mozilla Foundation, and
 * Opera Software ASA. You are granted a license to use, reproduce
 * and create derivative works of this document.
 */

[HTMLConstructor]
interface HTMLIFrameElement : HTMLElement {
  [SetterThrows, Pure, CEReactions]
           attribute DOMString src;
  [SetterThrows, Pure, CEReactions]
           attribute DOMString srcdoc;
  [SetterThrows, Pure, CEReactions]
           attribute DOMString name;
  [PutForwards=value, CEReactions] readonly attribute DOMTokenList sandbox;
           // attribute boolean seamless;
  [SetterThrows, Pure, CEReactions]
           attribute boolean allowFullscreen;
  [SetterThrows, Pure, CEReactions]
           attribute DOMString width;
  [SetterThrows, Pure, CEReactions]
           attribute DOMString height;
  [SetterThrows, Pure, Pref="network.http.enablePerElementReferrer", CEReactions]
           attribute DOMString referrerPolicy;
  [NeedsSubjectPrincipal]
  readonly attribute Document? contentDocument;
  readonly attribute WindowProxy? contentWindow;
};

// http://www.whatwg.org/specs/web-apps/current-work/#other-elements,-attributes-and-apis
partial interface HTMLIFrameElement {
  [SetterThrows, Pure, CEReactions]
           attribute DOMString align;
  [SetterThrows, Pure, CEReactions]
           attribute DOMString scrolling;
  [SetterThrows, Pure, CEReactions]
           attribute DOMString frameBorder;
  [SetterThrows, Pure, CEReactions]
           attribute DOMString longDesc;

  [TreatNullAs=EmptyString, SetterThrows, Pure, CEReactions]
          attribute DOMString marginHeight;
  [TreatNullAs=EmptyString, SetterThrows, Pure, CEReactions]
          attribute DOMString marginWidth;
};

partial interface HTMLIFrameElement {
  // GetSVGDocument
  [NeedsSubjectPrincipal]
  Document? getSVGDocument();
};

partial interface HTMLIFrameElement {
  // nsIDOMMozBrowserFrame
  [ChromeOnly,SetterThrows]
           attribute boolean mozbrowser;
};

HTMLIFrameElement implements MozFrameLoaderOwner;
HTMLIFrameElement implements BrowserElement;
