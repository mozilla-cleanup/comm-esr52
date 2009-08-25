/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Instantbird messenging client, released
 * 2007.
 *
 * The Initial Developer of the Original Code is
 * Florian QUEZE <florian@instantbird.org>.
 * Portions created by the Initial Developer are Copyright (C) 2007
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

const events = [
  "purple-quit"
];

var accountWizard = {
  onload: function aw_onload() {
    var protoList = document.getElementById("protolist");
    this.pcs = Components.classes["@instantbird.org/purple/core;1"]
                         .getService(Ci.purpleICoreService);
    var protos = [];
    for (let proto in this.getProtocols())
      protos.push(proto);
    protos.sort(function(a, b) a.name < b.name ? -1 : a.name > b.name ? 1 : 0);
    protos.forEach(function(proto) {
      var id = proto.id;
      var item = protoList.appendItem(proto.name, id, id);
      item.setAttribute("image", proto.iconBaseURI + "icon.png");
      item.setAttribute("class", "listitem-iconic");
    });

    // there is a strange selection bug without this timeout
    setTimeout(function() {
      protoList.selectedIndex = 0;
    }, 0);

    addObservers(this, events);
    window.addEventListener("unload", this.unload, false);
  },
  unload: function aw_unload() {
    removeObservers(accountWizard, events);
  },
  observe: function am_observe(aObject, aTopic, aData) {
    if (aTopic == "purple-quit") {
      // libpurple is being uninitialized. We can't create any new
      // account so keeping this wizard open would be pointless, close it.
      window.close();
    }
  },

  getUsername: function aw_getUsername() {
    // If the first username textbox is empty, make sure we return an empty
    // string so that it blocks the 'next' button of the wizard.
    if (!this.userNameBoxes[0].value)
      return "";

    return this.userNameBoxes.reduce(function(prev, elt) prev + elt.value, "");
  },

  checkUsername: function aw_checkUsername() {
    var wizard = document.getElementById("accountWizard");
    var name = accountWizard.getUsername();
    var duplicateWarning = document.getElementById("duplicateAccount");
    if (!name) {
      wizard.canAdvance = false;
      duplicateWarning.hidden = true;
      return;
    }

    var exists = accountWizard.proto.accountExists(name);
    wizard.canAdvance = !exists;
    duplicateWarning.hidden = !exists;
  },

  selectProtocol: function aw_selectProtocol() {
    var protoList = document.getElementById("protolist");
    var id = protoList.selectedItem.value;
    this.proto = this.pcs.getProtocolById(id);

    return true;
  },


  insertUsernameField: function aw_insertUsernameField(aName, aLabel, aParent,
                                                       aDefaultValue) {
    var hbox = document.createElement("hbox");
    hbox.setAttribute("id", aName + "-hbox");
    hbox.setAttribute("align", "baseline");
    setTimeout(this.fixBaseline, 0, hbox);
    hbox.setAttribute("equalsize", "always");

    var label = document.createElement("label");
    label.setAttribute("value", aLabel);
    label.setAttribute("control", aName);
    label.setAttribute("id", aName + "-label");
    hbox.appendChild(label);

    var textbox = document.createElement("textbox");
    textbox.setAttribute("id", aName);
    textbox.setAttribute("flex", 1);
    if (aDefaultValue)
      textbox.setAttribute("value", aDefaultValue);
    textbox.addEventListener("input", accountWizard.checkUsername, false);
    hbox.appendChild(textbox);

    aParent.appendChild(hbox);
    return textbox;
  },

  showUsernamePage: function aw_showUsernamePage() {
    var proto = this.proto.id;
    if (this.userNameProto == proto) {
      this.checkUsername();
      return;
    }

    var bundle = document.getElementById("accountsBundle");
    var usernameInfo;
    var emptyText = this.proto.usernameEmptyText;
    if (emptyText) {
      usernameInfo =
        bundle.getFormattedString("accountUsernameInfoWithDescription",
                                  [emptyText, this.proto.name]);
    }
    else {
      usernameInfo =
        bundle.getFormattedString("accountUsernameInfo", [this.proto.name]);
    }
    document.getElementById("usernameInfo").textContent = usernameInfo;

    var vbox = document.getElementById("userNameBox");
    // remove anything that may be there for another protocol
    var child;
    while (child = vbox.firstChild)
      vbox.removeChild(child);

    var splits = [];
    for (let split in this.getProtoUserSplits())
      splits.push(split);

    var label = bundle.getString("accountUsername");
    this.userNameBoxes = [this.insertUsernameField("name", label, vbox)];
    this.userNameBoxes[0].emptyText = emptyText;

    for (let i = 0; i < splits.length; ++i) {
      this.userNameBoxes.push({value: splits[i].separator});
      label = bundle.getFormattedString("accountColon", [splits[i].label]);
      let defaultVal = splits[i].defaultValue;
      this.userNameBoxes.push(this.insertUsernameField("username-split-" + i,
                                                       label, vbox,
                                                       defaultVal));
    }
    this.userNameBoxes[0].focus();
    this.userNameProto = proto;
    this.checkUsername();
  },

  hideUsernamePage: function aw_hideUsernamePage() {
    document.getElementById("accountWizard").canAdvance = true;
    var next = "account" +
      (this.proto.noPassword ? "advanced" : "password");
    setTimeout(this.fixBaseline, 0, document.getElementById("password"));
    document.getElementById("accountusername").next = next;
  },

  showAdvanced: function aw_showAdvanced() {
    // ensure we don't destroy user data if it's not necessary
    var id = this.proto.id;
    if (this.protoSpecOptId == id)
      return;
    this.protoSpecOptId = id;

/* FIXME
    document.getElementById("newMailNotification").hidden =
      !this.proto.newMailNotification;
*/
    this.populateProtoSpecificBox();

    this.proxy = Components.classes["@instantbird.org/purple/proxyinfo;1"]
                           .createInstance(Ci.purpleIProxyInfo);
    this.proxy.type = Ci.purpleIProxyInfo.useGlobal;
    this.displayProxyDescription();

    let alias = document.getElementById("alias");
    setTimeout(this.fixBaseline, 0, alias);
    alias.focus();
  },

  displayProxyDescription: function aw_displayProxyDescription() {
    var type = this.proxy.type;
    var bundle = document.getElementById("proxiesBundle");
    var proxy;
    var result;
    if (type == Ci.purpleIProxyInfo.useGlobal) {
      proxy = this.pcs.globalProxy;
      type = proxy.type;
    }
    else
      proxy = this.proxy;

    if (type == Ci.purpleIProxyInfo.noProxy)
      result = bundle.getString("proxies.directConnexion");

    if (type == Ci.purpleIProxyInfo.useEnvVar)
      result = bundle.getString("proxies.useEnvironemental");

    if (!result) {
      // At this point, we should have either a socks or http proxy
      var result;
      if (type == Ci.purpleIProxyInfo.httpProxy)
        result = bundle.getString("proxies.http");
      else if (type == Ci.purpleIProxyInfo.socks4Proxy)
        result = bundle.getString("proxies.socks4");
      else if (type == Ci.purpleIProxyInfo.socks5Proxy)
        result = bundle.getString("proxies.socks5");
      else
        throw "Unknown proxy type";

      if (result)
        result += " ";

      if (proxy.username)
        result += proxy.username + "@";

      result += proxy.host + ":" + proxy.port;
    }

    document.getElementById("proxyDescription").textContent = result;
  },

  createTextbox: function aw_createTextbox(aType, aValue, aLabel, aName) {
    var box = document.createElement("hbox");
    box.setAttribute("align", "baseline");
    setTimeout(this.fixBaseline, 0, box);
    box.setAttribute("equalsize", "always");

    var label = document.createElement("label");
    label.setAttribute("value", aLabel);
    label.setAttribute("control", aName);
    box.appendChild(label);

    var textbox = document.createElement("textbox");
    if (aType)
      textbox.setAttribute("type", aType);
    textbox.setAttribute("value", aValue);
    textbox.setAttribute("id", aName);
    textbox.setAttribute("flex", "1");

    box.appendChild(textbox);
    return box;
  },

  createMenulist: function aw_createMenulist(aList, aLabel, aName) {
    var box = document.createElement("hbox");
    box.setAttribute("align", "baseline");
    setTimeout(this.fixBaseline, 0, box);

    var label = document.createElement("label");
    label.setAttribute("value", aLabel);
    label.setAttribute("control", aName);
    box.appendChild(label);

    aList.QueryInterface(Ci.nsISimpleEnumerator);
    var menulist = document.createElement("menulist");
    menulist.setAttribute("id", aName);
    var popup = menulist.appendChild(document.createElement("menupopup"));
    while (aList.hasMoreElements()) {
      let elt = aList.getNext();
      let item = document.createElement("menuitem");
      item.setAttribute("label", elt.name);
      item.setAttribute("value", elt.value);
      popup.appendChild(item);
    }
    box.appendChild(menulist);
    return box;
  },

  populateProtoSpecificBox: function aw_populate() {
    var id = this.proto.id;
    var box = document.getElementById("protoSpecific");
    var bundle = document.getElementById("accountsBundle");
    document.getElementById("protoSpecificCaption").label =
      bundle.getFormattedString("protoOptions", [this.proto.name]);
    var child;
    while (child = box.firstChild)
      box.removeChild(child);
    for (let opt in this.getProtoOptions()) {
      var text = opt.label;
      var name = id + "-" + opt.name;
      switch (opt.type) {
      case opt.typeBool:
        var chk = document.createElement("checkbox");
        if (opt.getBool())
          chk.setAttribute("checked", "true");
        chk.setAttribute("label", text);
        chk.setAttribute("id", name);
        box.appendChild(chk);
        break;
      case opt.typeInt:
        box.appendChild(this.createTextbox("number", opt.getInt(),
                                           text, name));
        break;
      case opt.typeString:
        box.appendChild(this.createTextbox(null, opt.getString(),
                                           text, name));
        break;
      case opt.typeList:
        box.appendChild(this.createMenulist(opt.getList(), text, name));
        break;
      default:
        throw "unknown preference type " + opt.type;
      }
    }
  },

  fixBaseline: function aw_fixBaseline(aElement) {
    // Remove this when bug 494901 is fixed
    aElement.setAttribute("align", "start");
    aElement.setAttribute("align", "baseline");
  },

  createSummaryRow: function aw_createSummaryRow(aLabel, aValue) {
    var row = document.createElement("row");
    row.setAttribute("align", "baseline");
    setTimeout(this.fixBaseline, 0, row);

    var label = document.createElement("label");
    label.setAttribute("class", "header");
    if (aLabel.length > 20) {
      aLabel = aLabel.substring(0, 20);
      aLabel += "…";
    }
    label.setAttribute("value", aLabel);
    row.appendChild(label);

    var textbox = document.createElement("textbox");
    textbox.setAttribute("value", aValue);
    textbox.setAttribute("class", "plain");
    textbox.setAttribute("readonly", true);
    row.appendChild(textbox);

    return row;
  },

  showSummary: function aw_showSummary() {
    var rows = document.getElementById("summaryRows");
    var bundle = document.getElementById("accountsBundle");
    var child;
    while (child = rows.firstChild)
      rows.removeChild(child);

    var label = document.getElementById("protoLabel").value;
    rows.appendChild(this.createSummaryRow(label, this.proto.name));
    this.username = this.getUsername();
    label = bundle.getString("accountUsername");
    rows.appendChild(this.createSummaryRow(label, this.username));
    if (!this.proto.noPassword) {
      this.password = this.getValue("password");
      label = document.getElementById("passwordLabel").value;
      var pass = "";
      for (let i = 0; i < this.password.length; ++i)
        pass += "*";
      rows.appendChild(this.createSummaryRow(label, pass));
    }
    this.alias = this.getValue("alias");
    if (this.alias) {
      label = document.getElementById("aliasLabel").value;
      rows.appendChild(this.createSummaryRow(label, this.alias));
    }

/* FIXME
    if (this.proto.newMailNotification)
      rows.appendChild(this.createSummaryRow("Notify of new mails:",
                                             this.getValue("newMailNotification")));
*/

    var id = this.proto.id;
    this.prefs = [ ];
    for (let opt in this.getProtoOptions()) {
      let name = opt.name;
      let val = this.getValue(id + "-" + name);
      // The value will be undefined if the proto specific groupbox has never been opened
      if (val === undefined)
        continue;
      switch (opt.type) {
      case opt.typeBool:
        if (val != opt.getBool())
          this.prefs.push({opt: opt, name: name, value: val});
        break;
      case opt.typeInt:
        if (val != opt.getInt())
          this.prefs.push({opt: opt, name: name, value: val});
        break;
      case opt.typeString:
        if (val != opt.getString())
          this.prefs.push({opt: opt, name: name, value: val});
        break;
      case opt.typeList:
        var list = opt.getList().QueryInterface(Ci.nsISimpleEnumerator);
        var defaultVal = list.hasMoreElements() &&
          list.getNext().value || "";
        if (val != defaultVal)
          this.prefs.push({opt: opt, name: name, value: val});
        break;
      default:
        throw "unknown preference type " + opt.type;
      }
    }

    for (let i = 0; i < this.prefs.length; ++i) {
      let opt = this.prefs[i];
      let label = bundle.getFormattedString("accountColon", [opt.opt.label]);
      rows.appendChild(this.createSummaryRow(label, opt.value));
    }
  },

  createAccount: function aw_createAccount() {
    var acc = this.pcs.createAccount(this.username, this.proto.id);
    if (!this.proto.noPassword) {
      acc.password = this.password;
      acc.rememberPassword = true;
    }
    if (this.alias)
      acc.alias = this.alias;
    //FIXME: newMailNotification

    for (let i = 0; i < this.prefs.length; ++i) {
      let option = this.prefs[i];
      let opt = option.opt;
      switch(opt.type) {
      case opt.typeBool:
        acc.setBool(option.name, option.value);
        break;
      case opt.typeInt:
        acc.setInt(option.name, option.value);
        break;
      case opt.typeString:
      case opt.typeList:
        acc.setString(option.name, option.value);
        break;
      default:
        throw "unknown type";
      }
    }
    var autologin = this.getValue("connectNow");
    acc.autoLogin = autologin;

    acc.proxyInfo = this.proxy;
    acc.save();

    try {
      if (autologin)
        acc.connect();
    } catch (e) {
      // If the connection fails (for example if we are currently in
      // offline mode), we still want to close the account wizard
    }

    if (window.opener) {
      var am = window.opener.gAccountManager;
      if (am)
        am.selectAccount(acc.id);
    }

    return true;
  },

  getValue: function aw_getValue(aId) {
    var elt = document.getElementById(aId);
    if ("checked" in elt)
      return elt.checked;
    return elt.value;
  },

  getProtocols: function aw_getProtocols() {
    return getIter(this.pcs.getProtocols());
  },
  getProtoOptions: function aw_getProtoOptions() {
    return getIter(this.proto.getOptions());
  },
  getProtoUserSplits: function aw_getProtoUserSplits() {
    return getIter(this.proto.getUsernameSplit());
  },

  onGroupboxKeypress: function aw_onGroupboxKeypress(aEvent) {
    var target = aEvent.target;
    var code = aEvent.charCode || aEvent.keyCode;
    if (code == KeyEvent.DOM_VK_SPACE ||
        (code == KeyEvent.DOM_VK_LEFT && !target.hasAttribute("closed")) ||
	(code == KeyEvent.DOM_VK_RIGHT && target.hasAttribute("closed")))
        this.toggleGroupbox(target.id);
  },

  toggleGroupbox: function aw_toggleGroupbox(id) {
    var elt = document.getElementById(id);
    if (elt.hasAttribute("closed")) {
      elt.removeAttribute("closed");
      if (elt.flexWhenOpened)
        elt.flex = elt.flexWhenOpened;
    }
    else {
      elt.setAttribute("closed", "true");
      if (elt.flex) {
        elt.flexWhenOpened = elt.flex;
        elt.flex = 0;
      }
    }
  },

  openProxySettings: function aw_openProxySettings() {
    window.openDialog("chrome://instantbird/content/proxies.xul", "",
                      "chrome,modal,titlebar,centerscreen",
                      this);
    this.displayProxyDescription();
  }
};
