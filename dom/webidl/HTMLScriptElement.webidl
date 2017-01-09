/* -*- Mode: IDL; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The origin of this IDL file is
 * http://www.whatwg.org/specs/web-apps/current-work/#the-script-element
 * http://www.whatwg.org/specs/web-apps/current-work/#other-elements,-attributes-and-apis
 */

[HTMLConstructor]
interface HTMLScriptElement : HTMLElement {
  [SetterThrows, CEReactions]
  attribute DOMString src;
  [SetterThrows, CEReactions]
  attribute DOMString type;
  [SetterThrows, CEReactions]
  attribute DOMString charset;
  [SetterThrows, CEReactions]
  attribute boolean async;
  [SetterThrows, CEReactions]
  attribute boolean defer;
  [SetterThrows, CEReactions]
  attribute DOMString? crossOrigin;
  [SetterThrows, CEReactions]
  attribute DOMString text;
};

// http://www.whatwg.org/specs/web-apps/current-work/#other-elements,-attributes-and-apis
partial interface HTMLScriptElement {
  [SetterThrows, CEReactions]
  attribute DOMString event;
  [SetterThrows, CEReactions]
  attribute DOMString htmlFor;
};

// https://w3c.github.io/webappsec/specs/subresourceintegrity/#htmlscriptelement-1
partial interface HTMLScriptElement {
  [SetterThrows, CEReactions]
  attribute DOMString integrity;
};
