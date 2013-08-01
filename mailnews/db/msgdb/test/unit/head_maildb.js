Components.utils.import("resource://gre/modules/Services.jsm");
Components.utils.import("resource:///modules/mailServices.js");
Components.utils.import("resource://gre/modules/XPCOMUtils.jsm");
Components.utils.import("resource://testing-common/mailnews/mailDirService.js");
Components.utils.import("resource://testing-common/mailnews/mailTestUtils.js");

var Cc = Components.classes;
var Ci = Components.interfaces;
var Cr = Components.results;
var CC = Components.Constructor;

var gProfileDir = ProfileDir.initialize(do_get_profile());

// Import the main scripts that mailnews tests need to set up and tear down
load("../../../../resources/localAccountUtils.js");
