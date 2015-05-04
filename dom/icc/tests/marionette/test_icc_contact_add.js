/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */

MARIONETTE_TIMEOUT = 60000;
MARIONETTE_HEAD_JS = "head.js";

function testAddContact(aIcc, aType, aPin2) {
  log("testAddContact: type=" + aType + ", pin2=" + aPin2);
  let contact = new mozContact({
    name: ["add"],
    tel: [{value: "0912345678"}],
    email:[]
  });

  return aIcc.updateContact(aType, contact, aPin2)
    .then((aResult) => {
      is(aResult.id, aIcc.iccInfo.iccid + "5");
      is(aResult.name[0], "add");
      is(aResult.tel[0].value, "0912345678");
      // Get ICC contact for checking new contact
      return aIcc.readContacts(aType)
        .then((aResult) => {
          // There are 4 SIM contacts which are harded in emulator
          is(aResult.length, 5);

          is(aResult[4].name[0], "add");
          is(aResult[4].tel[0].value, "0912345678");
          is(aResult[4].id, aIcc.iccInfo.iccid + "5");
        }, (aError) => {
          ok(false, "Cannot get " + aType + " contacts: " + aError.name);
        })
    }, (aError) => {
      if (aType === "fdn" && aPin2 === undefined) {
        ok(aError.name === "SimPin2",
           "expected error when pin2 is not provided");
      } else {
        ok(false, "Cannot add " + aType + " contact: " + aError.name);
      }
    });
}

function tearDownContact(aIcc, aContactId, aType, aPin2) {
  log("tearDownContact: contactId=" + aContactId + ", type=" + aType + ", pin2=" + aPin2);
  let contact = {
    id: aIcc.iccInfo.iccid + aContactId,
  };

  return aIcc.updateContact(aType, contact, aPin2)
    .then((aResult) => {
      // Get ICC contact for checking new contact
      return aIcc.readContacts(aType)
        .then((aResult) => {
          // There are 4 SIM contacts which are harded in emulator
          is(aResult.length, 4);
        }, (aError) => {
          ok(false, "Cannot get " + aType + " contacts: " + aError.name);
        })
    }, (aError) => {
      if (aType === "fdn" && aPin2 === undefined) {
        ok(aError.name === "SimPin2",
           "expected error when pin2 is not provided");
      } else {
        ok(false, "Cannot tear down " + aType + " contact: " + aError.name);
      }
    });
}

// Start tests
startTestCommon(function() {
  let icc = getMozIcc();

    // Test add adn contacts
  return testAddContact(icc, "adn")
    // Test add fdn contacts
    .then(() => testAddContact(icc, "fdn", "0000"))
    // Test add fdn contacts without passing pin2
    .then(() => testAddContact(icc, "fdn"))
    .then(() => tearDownContact(icc, "5", "adn"))
    .then(() => tearDownContact(icc, "5", "fdn"))
    .then(() => tearDownContact(icc, "5", "fdn", "0000"));
});