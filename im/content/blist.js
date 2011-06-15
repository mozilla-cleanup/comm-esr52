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

Components.utils.import("resource:///modules/imStatusUtils.jsm");

const events = ["contact-availability-changed",
                "contact-added",
                "contact-moved",
                "contact-tagged",
                "status-changed",
                "tag-hidden",
                "tag-shown",
                "user-display-name-changed",
                "user-icon-changed",
                "purple-quit"];

const showOfflineBuddiesPref = "messenger.buddies.showOffline";

var gBuddyListContextMenu = null;

function buddyListContextMenu(aXulMenu) {
  this.target  = document.popupNode;
  this.menu    = aXulMenu;
  let localName = this.target.localName;
  this.onContact = localName == "contact";
  this.onBuddy = localName == "buddy";
  this.onGroup = localName == "group";
  this.shouldDisplay = true;

  let hide = !(this.onContact || this.onBuddy);
  [ "context-openconversation",
    "context-showlogs",
    "context-edit-buddy-separator",
    "context-alias",
    "context-delete",
    "context-tags",
    "context-show-offline-buddies-separator"
  ].forEach(function (aId) {
    document.getElementById(aId).hidden = hide;
  });

  [ "context-hide-tag",
    "context-visible-tags"
  ].forEach(function (aId) {
    document.getElementById(aId).hidden = !this.onGroup;
  }, this);

  if (this.onGroup) {
    document.getElementById("context-hide-tag").disabled =
      this.target.tag.id == -1;
  }

  document.getElementById("context-show-offline-buddies-separator").hidden =
    hide && !this.onGroup;

  let detach = document.getElementById("context-detach");
  detach.hidden = !this.onBuddy;
  if (this.onBuddy)
    detach.disabled = this.target.buddy.contact.getBuddies().length == 1;

  document.getElementById("context-openconversation").disabled =
    !hide && !this.target.canOpenConversation();
}

// Prototype for buddyListContextMenu "class."
buddyListContextMenu.prototype = {
  openConversation: function blcm_openConversation() {
    if (this.onContact || this.onBuddy)
      this.target.openConversation();
  },
  alias: function blcm_alias() {
    if (this.onContact)
      this.target.startAliasing();
    else if (this.onBuddy)
      this.target.contact.startAliasing();
  },
  detach: function blcm_detach() {
    if (!this.onBuddy)
      return;

    let buddy = this.target.buddy;
    buddy.contact.detachBuddy(buddy);
  },
  delete: function blcm_delete() {
    let buddy;
    if (this.onContact)
      buddy = this.target.contact.preferredBuddy;
    else if (this.onBuddy)
      buddy = this.target.buddy;
    else
      return;

    let bundle = document.getElementById("instantbirdBundle").stringBundle;
    let displayName = this.target.displayName;
    let promptTitle = bundle.formatStringFromName("buddy.deletePrompt.title",
                                                  [displayName], 1);
    let userName = buddy.userName;
    if (displayName != userName)
      displayName += " (" + userName + ")";
    let proto = buddy.protocol.name; // FIXME build a list
    let promptMessage = bundle.formatStringFromName("buddy.deletePrompt.message",
                                                    [displayName, proto], 2);
    let deleteButton = bundle.GetStringFromName("buddy.deletePrompt.button");
    let prompts = Services.prompt;
    let flags = prompts.BUTTON_TITLE_IS_STRING * prompts.BUTTON_POS_0 +
                prompts.BUTTON_TITLE_CANCEL * prompts.BUTTON_POS_1 +
                prompts.BUTTON_POS_1_DEFAULT;
    if (prompts.confirmEx(window, promptTitle, promptMessage, flags,
                          deleteButton, null, null, null, {}))
      return;

    this.target.remove();
  },
  tagsPopupShowing: function blcm_tagsPopupShowing() {
    if (!this.onContact && !this.onBuddy)
      return;

    let popup = document.getElementById("context-tags-popup");
    let item;
    while ((item = popup.firstChild) && item.localName != "menuseparator")
      popup.removeChild(item);

    let contact = (this.onBuddy ? this.target.contact : this.target).contact;
    let tags = contact.getTags();
    let groupId =
      (this.onBuddy ? this.target.contact : this.target).group.groupId;
    let sortFunction = function (a, b) {
      let [a, b] = [a.name.toLowerCase(), b.name.toLowerCase()];
      return a < b ? 1 : a > b ? -1 : 0;
    };
    Services.tags.getTags()
            .sort(sortFunction)
            .forEach(function (aTag) {
      item = document.createElement("menuitem");
      item.setAttribute("label", aTag.name);
      item.setAttribute("type", "checkbox");
      let id = aTag.id;
      item.groupId = id;
      if (tags.some(function (t) t.id == id)) {
        item.setAttribute("checked", "true");
        if (tags.length == 1)
          item.setAttribute("disabled", "true"); // can't remove the last tag.
      }
      popup.insertBefore(item, popup.firstChild);
    });
  },
  tag: function blcm_tag(aEvent) {
    let id = aEvent.originalTarget.groupId;
    if (!id)
      return;

    let tag = Services.tags.getTagById(id);
    let contact = (this.onBuddy ? this.target.contact : this.target).contact;
    if (contact.getTags().some(function (t) t.id == id))
      contact.removeTag(tag);
    else
      contact.addTag(tag);
  },
  addNewTag: function blcm_addNewTag() {
    let bundle = document.getElementById("instantbirdBundle").stringBundle;
    let title = bundle.GetStringFromName("newTagPromptTitle");
    let message = bundle.GetStringFromName("newTagPromptMessage");
    let name = {};
    if (!Services.prompt.prompt(window, title, message, name, null,
                                {value: false}) || !name.value)
      return; // the user canceled

    let contact = (this.onBuddy ? this.target.contact : this.target).contact;
    // If the tag already exists, createTag will return it, and if the
    // contact already has it, addTag will return early.
    contact.addTag(Services.tags.createTag(name.value));
  },
  showLogs: function blcm_showLogs() {
    let enumerator;
    if (this.onContact)
      enumerator = Services.logs.getLogsForContact(this.target.contact);
    else if (this.onBuddy)
      enumerator = Services.logs.getLogsForBuddy(this.target.buddy);
    else
      return;

    var logs = [];
    for (let log in getIter(enumerator))
      logs.push(log);
    window.openDialog("chrome://instantbird/content/viewlog.xul",
                      "Logs", "chrome,resizable", {logs: logs},
                      this.target.displayName);
  },
  hideTag: function blcm_hideTag() {
    if (!this.onGroup || this.target.tag.id == -1)
      return;

    this.target.hide();
  },
  visibleTagsPopupShowing: function blcm_visibleTagsPopupShowing() {
    if (!this.onGroup)
      return;

    let popup = document.getElementById("context-visible-tags-popup");
    let item;
    while ((item = popup.firstChild) && item.localName != "menuseparator")
      popup.removeChild(item);

    let sortFunction = function (a, b) {
      let [a, b] = [a.name.toLowerCase(), b.name.toLowerCase()];
      return a < b ? 1 : a > b ? -1 : 0;
    };
    Services.tags.getTags()
            .sort(sortFunction)
            .forEach(function (aTag) {
      item = document.createElement("menuitem");
      item.setAttribute("label", aTag.name);
      item.setAttribute("type", "checkbox");
      let id = aTag.id;
      item.groupId = id;
      if (!Services.tags.isTagHidden(aTag))
        item.setAttribute("checked", "true");
      popup.insertBefore(item, popup.firstChild);
    });

    let otherContactsTag = document.getElementById("group-1");
    [ "context-other-contacts-tag-separator",
      "context-other-contacts-tag"
    ].forEach(function (aId) {
      document.getElementById(aId).hidden = !otherContactsTag;
    });
    if (otherContactsTag) {
      // This avoids having the localizable "Other Contacts" string in
      // both a .dtd and .properties file.
      document.getElementById("context-other-contacts-tag").label =
        otherContactsTag.displayName;
    }
  },
  visibleTags: function blcm_visibleTags(aEvent) {
    let id = aEvent.originalTarget.groupId;
    if (!id)
      return;
    let tags = Services.tags;
    let tag = tags.getTagById(id);
    if (tags.isTagHidden(tag))
      tags.showTag(tag);
    else
      tags.hideTag(tag);
  },
  toggleShowOfflineBuddies: function blcm_toggleShowOfflineBuddies() {
    let newValue =
      !!document.getElementById("context-show-offline-buddies")
                .getAttribute("checked");
    Services.prefs.setBoolPref(showOfflineBuddiesPref, newValue);
  }
};

var buddyList = {
  observe: function bl_observe(aSubject, aTopic, aMsg) {
    if (aTopic == "purple-quit") {
      window.close();
      return;
    }

    if (aTopic == "nsPref:changed" && aMsg == showOfflineBuddiesPref) {
      let showOffline = Services.prefs.getBoolPref(showOfflineBuddiesPref);
      this._showOffline = showOffline;
      let item = document.getElementById("context-show-offline-buddies");
      if (showOffline)
        item.setAttribute("checked", "true");
      else
        item.removeAttribute("checked");

      Services.tags.getTags().forEach(function (aTag) {
        let elt = document.getElementById("group" + aTag.id);
        if (elt)
          elt.showOffline = showOffline;
        else if (showOffline) {
          if (Services.tags.isTagHidden(aTag))
            this.showOtherContacts();
          else
            this.displayGroup(aTag);
        }
      }, this);
      let elt = document.getElementById("group-1"); // "Other contacts"
      if (elt)
        elt.showOffline = showOffline;
      return;
    }

    if (aTopic == "status-changed") {
      this.displayCurrentStatus();
      return;
    }

    if (aTopic == "tag-hidden") {
      this.showOtherContacts();
      return;
    }

    if (aTopic == "tag-shown") {
      if (!document.getElementById("group" + aSubject.id))
        this.displayGroup(aSubject);
      return;
    }

    if (aTopic == "user-icon-changed") {
      this.displayUserIcon();
      return;
    }

    if (aTopic == "user-display-name-changed") {
      this.displayUserDisplayName();
      return;
    }

    // aSubject is an imIContact
    if (aSubject.online || this._showOffline) {
      aSubject.getTags().forEach(function (aTag) {
        if (Services.tags.isTagHidden(aTag))
          this.showOtherContacts();
        else if (!document.getElementById("group" + aTag.id))
          this.displayGroup(aTag);
      }, this);
    }
  },

  displayUserIcon: function bl_displayUserIcon() {
    let icon = Services.core.getUserIcon();
    document.getElementById("userIcon").src = icon ? icon.spec : "";
  },

  displayUserDisplayName: function bl_displayUserDisplayName() {
    let displayName = Services.core.userDisplayName;
    let elt = document.getElementById("displayName");
    if (displayName)
      elt.removeAttribute("usingDefault");
    else {
      let bundle = document.getElementById("instantbirdBundle");
      displayName = bundle.getString("displayNameEmptyText");
      elt.setAttribute("usingDefault", displayName);
    }
    elt.setAttribute("value", displayName);
  },

  displayStatusType: function bl_displayStatusType(aStatusType) {
    document.getElementById("statusMessage")
            .setAttribute("statusType", aStatusType);
    let statusString = Status.toLabel(aStatusType);
    let statusTypeIcon = document.getElementById("statusTypeIcon");
    statusTypeIcon.setAttribute("status", aStatusType);
    statusTypeIcon.setAttribute("tooltiptext", statusString);
    return statusString;
  },

  displayCurrentStatus: function bl_displayCurrentStatus() {
    let pcs = Services.core;
    let status = Status.toAttribute(pcs.currentStatusType);
    let message = status == "offline" ? "" : pcs.currentStatusMessage;
    let statusString = this.displayStatusType(status);
    let statusMessage = document.getElementById("statusMessage");
    if (message)
      statusMessage.removeAttribute("usingDefault");
    else {
      statusMessage.setAttribute("usingDefault", statusString);
      message = statusString;
    }
    statusMessage.setAttribute("value", message);
    statusMessage.setAttribute("tooltiptext", message);
  },

  editStatus: function bl_editStatus(aEvent) {
    let status = aEvent.originalTarget.getAttribute("status");
    if (status == "offline")
      Services.core.setStatus(Ci.imIStatusInfo.STATUS_OFFLINE, "");
    else if (status)
      this.startEditStatus(status);
  },

  startEditStatus: function bl_startEditStatus(aStatusType) {
    let currentStatusType =
      document.getElementById("statusTypeIcon").getAttribute("status");
    if (aStatusType != currentStatusType) {
      this._statusTypeBeforeEditing = currentStatusType;
      this._statusTypeEditing = aStatusType;
      this.displayStatusType(aStatusType);
    }
    this.statusMessageClick();
  },

  statusMessageClick: function bl_statusMessageClick() {
    let statusType =
      document.getElementById("statusTypeIcon").getAttribute("status");
    if (statusType == "offline")
      return;

    let elt = document.getElementById("statusMessage");
    if (!elt.hasAttribute("editing")) {
      elt.setAttribute("editing", "true");
      elt.addEventListener("keypress", this.statusMessageKeyPress, false);
      elt.addEventListener("blur", this.statusMessageBlur, false);
      if (elt.hasAttribute("usingDefault")) {
        if ("_statusTypeBeforeEditing" in this &&
            this._statusTypeBeforeEditing == "offline")
          elt.setAttribute("value", Services.core.currentStatusMessage);
        else
          elt.removeAttribute("value");
      }
      if (!("TextboxSpellChecker" in window))
        Components.utils.import("resource:///modules/imTextboxUtils.jsm");
      TextboxSpellChecker.registerTextbox(elt);
      // force binding attachmant by forcing layout
      elt.getBoundingClientRect();
      elt.select();
    }

    this.statusMessageRefreshTimer();
  },

  statusMessageRefreshTimer: function bl_statusMessageRefreshTimer() {
    const timeBeforeAutoValidate = 20 * 1000;
    if ("_stopEditStatusTimeout" in this)
      clearTimeout(this._stopEditStatusTimeout);
    this._stopEditStatusTimeout = setTimeout(this.finishEditStatusMessage,
                                             timeBeforeAutoValidate, true);
  },

  statusMessageBlur: function bl_statusMessageBlur(aEvent) {
    if (aEvent.originalTarget == document.getElementById("statusMessage").inputField)
      buddyList.finishEditStatusMessage(true);
  },

  statusMessageKeyPress: function bl_statusMessageKeyPress(aEvent) {
    switch (aEvent.keyCode) {
      case aEvent.DOM_VK_RETURN:
      case aEvent.DOM_VK_ENTER:
        buddyList.finishEditStatusMessage(true);
        break;

      case aEvent.DOM_VK_ESCAPE:
        buddyList.finishEditStatusMessage(false);
        break;

      default:
        buddyList.statusMessageRefreshTimer();
    }
  },

  finishEditStatusMessage: function bl_finishEditStatusMessage(aSave) {
    clearTimeout(this._stopEditStatusTimeout);
    delete this._stopEditStatusTimeout;
    let elt = document.getElementById("statusMessage");
    if (aSave) {
      let newStatus = Ci.imIStatusInfo.STATUS_UNKNOWN;
      if ("_statusTypeEditing" in this) {
        let statusType = this._statusTypeEditing;
        if (statusType == "available")
          newStatus = Ci.imIStatusInfo.STATUS_AVAILABLE;
        else if (statusType == "unavailable")
          newStatus = Ci.imIStatusInfo.STATUS_UNAVAILABLE;
        else if (statusType == "offline")
          newStatus = Ci.imIStatusInfo.STATUS_OFFLINE;
        delete this._statusTypeBeforeEditing;
        delete this._statusTypeEditing;
      }
      // apply the new status only if it is different from the current one
      if (newStatus != Ci.imIStatusInfo.STATUS_UNKNOWN ||
          elt.value != elt.getAttribute("value"))
        Services.core.setStatus(newStatus, elt.value);
    }
    else if ("_statusTypeBeforeEditing" in this) {
      this.displayStatusType(this._statusTypeBeforeEditing);
      delete this._statusTypeBeforeEditing;
      delete this._statusTypeEditing;
    }

    if (elt.hasAttribute("usingDefault"))
      elt.setAttribute("value", elt.getAttribute("usingDefault"));
    TextboxSpellChecker.unregisterTextbox(elt);
    elt.removeAttribute("editing");
    elt.removeEventListener("keypress", this.statusMessageKeyPress, false);
    elt.removeEventListener("blur", this.statusMessageBlur, false);
  },

  userIconClick: function bl_userIconClick() {
    const nsIFilePicker = Components.interfaces.nsIFilePicker;
    let fp = Components.classes["@mozilla.org/filepicker;1"]
                       .createInstance(nsIFilePicker);
    let bundle = document.getElementById("instantbirdBundle");
    fp.init(window, bundle.getString("userIconFilePickerTitle"),
            nsIFilePicker.modeOpen);
    fp.appendFilters(nsIFilePicker.filterImages);
    if (fp.show() == nsIFilePicker.returnOK)
      Services.core.setUserIcon(fp.file);
  },

  displayNameClick: function bl_displayNameClick() {
    let elt = document.getElementById("displayName");
    if (!elt.hasAttribute("editing")) {
      elt.setAttribute("editing", "true");
      if (elt.hasAttribute("usingDefault"))
        elt.removeAttribute("value");
      elt.addEventListener("keypress", this.displayNameKeyPress, false);
      elt.addEventListener("blur", this.displayNameBlur, false);
      // force binding attachmant by forcing layout
      elt.getBoundingClientRect();
      elt.select();
    }

    this.displayNameRefreshTimer();
  },

  displayNameRefreshTimer: function bl_displayNameRefreshTimer() {
    const timeBeforeAutoValidate = 20 * 1000;
    clearTimeout(this._stopEditDisplayNameTimeout);
    this._stopEditDisplayNameTimeout =
      setTimeout(this.finishEditDisplayName, timeBeforeAutoValidate, true);
  },

  displayNameBlur: function bl_displayNameBlur(aEvent) {
    if (aEvent.originalTarget == document.getElementById("displayName").inputField)
      buddyList.finishEditDisplayName(true);
  },

  displayNameKeyPress: function bl_displayNameKeyPress(aEvent) {
    switch (aEvent.keyCode) {
      case aEvent.DOM_VK_RETURN:
      case aEvent.DOM_VK_ENTER:
        buddyList.finishEditDisplayName(true);
        break;

      case aEvent.DOM_VK_ESCAPE:
        buddyList.finishEditDisplayName(false);
        break;

      default:
        buddyList.displayNameRefreshTimer();
    }
  },

  finishEditDisplayName: function bl_finishEditDisplayName(aSave) {
    clearTimeout(this._stopEditDisplayNameTimeout);
    let elt = document.getElementById("displayName");
    // Apply the new display name only if it is different from the current one.
    if (aSave && elt.value != elt.getAttribute("value"))
      Services.core.userDisplayName = elt.value;
    else if (elt.hasAttribute("usingDefault"))
      elt.setAttribute("value", elt.getAttribute("usingDefault"));

    elt.removeAttribute("editing");
    elt.removeEventListener("keypress", this.displayNameKeyPress, false);
    elt.removeEventListener("blur", this.displayNameBlur, false);
  },

  load: function bl_load() {
    var blistWindows = Services.wm.getEnumerator("Messenger:blist");
    while (blistWindows.hasMoreElements()) {
      var win = blistWindows.getNext();
      if (win != window) {
        win.QueryInterface(Ci.nsIDOMWindowInternal).focus();
        window.close();
        return;
      }
    }

    // TODO remove this once we cleanup the way the menus are inserted
    let menubar = document.getElementById("blistMenubar");
    let statusArea = document.getElementById("statusArea");
    statusArea.parentNode.insertBefore(menubar, statusArea);

    buddyList.displayCurrentStatus();
    buddyList.displayUserDisplayName();
    buddyList.displayUserIcon();

    let prefBranch = Services.prefs;
    buddyList._showOffline = prefBranch.getBoolPref(showOfflineBuddiesPref);
    if (buddyList._showOffline) {
      document.getElementById("context-show-offline-buddies")
              .setAttribute("checked", "true");
    }

    let blistBox = document.getElementById("buddylistbox");
    blistBox.removeGroup = function(aGroupElt) {
      let index = buddyList._displayedGroups.indexOf(aGroupElt);
      if (index != -1)
        buddyList._displayedGroups.splice(index, 1);
      this.removeChild(aGroupElt);
    };
    let showOtherContacts = false;
    Services.tags.getTags().forEach(function (aTag) {
      if (Services.tags.isTagHidden(aTag))
        showOtherContacts = true;
      else
        buddyList.displayGroup(aTag);
    });
    if (showOtherContacts)
      buddyList.showOtherContacts();
    blistBox.focus();

    prefBranch.addObserver(showOfflineBuddiesPref, buddyList, false);
    addObservers(buddyList, events);

    this.addEventListener("unload", buddyList.unload, false);
  },
  _displayedGroups: [],
  _getGroupIndex: function(aName) {
    let start = 0;
    let end = this._displayedGroups.length;
    let name = aName.toLowerCase();
    while (start < end) {
      let middle = start + Math.floor((end - start) / 2);
      if (name < this._displayedGroups[middle].displayName.toLowerCase())
        end = middle;
      else
        start = middle + 1;
    }
    return end;
  },
  displayGroup: function(aTag) {
    let blistBox = document.getElementById("buddylistbox");
    let groupElt = document.createElement("group");
    let index;
    let ref = null;
    if (aTag.id != -1) {
      index = this._getGroupIndex(aTag.name);
      if (index == this._displayedGroups.length)
        ref = document.getElementById("group-1"); // 'Other Contacts'
      else
        ref = this._displayedGroups[index];
    }
    blistBox.insertBefore(groupElt, ref);

    if (this._showOffline)
      groupElt._showOffline = true;
    if (!groupElt.build(aTag))
      blistBox.removeChild(groupElt);
    else if (index !== undefined)
      this._displayedGroups.splice(index, 0, groupElt);
  },
  showOtherContacts: function bl_showOtherContacts() {
    if (!document.getElementById("group-1"))
      this.displayGroup(Services.tags.otherContactsTag);
  },
  unload: function bl_unload() {
    removeObservers(buddyList, events);
    Services.prefs.removeObserver(showOfflineBuddiesPref, buddyList);
   },

  // Handle key pressing
  keyPress: function bl_keyPress(aEvent) {
    var item = document.getElementById("buddylistbox").selectedItem;
    if (!item || !item.parentNode) // empty list or item no longer in the list
      return;
    item.keyPress(aEvent);
  }
};

this.addEventListener("load", buddyList.load, false);
