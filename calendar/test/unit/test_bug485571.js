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
 * The Original Code is Mozilla Calendar tests code.
 *
 * The Initial Developer of the Original Code is
 *   Stefan Sitter <ssitter@gmail.com>.
 * Portions created by the Initial Developer are Copyright (C) 2009
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

function run_test() {
    // Check that the RELATED property is correctly set
    // after parsing the given VALARM component

    // trigger set 15 minutes prior to the start of the event
    check_relative("BEGIN:VALARM\n" +
                   "ACTION:DISPLAY\n" +
                   "TRIGGER:-PT15M\n" +
                   "DESCRIPTION:TEST\n" +
                   "END:VALARM",
                   Ci.calIAlarm.ALARM_RELATED_START);

    // trigger set 15 minutes prior to the start of the event
    check_relative("BEGIN:VALARM\n" +
                   "ACTION:DISPLAY\n" +
                   "TRIGGER;VALUE=DURATION:-PT15M\n" +
                   "DESCRIPTION:TEST\n" +
                   "END:VALARM",
                   Ci.calIAlarm.ALARM_RELATED_START);

    // trigger set 15 minutes prior to the start of the event
    check_relative("BEGIN:VALARM\n" +
                   "ACTION:DISPLAY\n" +
                   "TRIGGER;RELATED=START:-PT15M\n" +
                   "DESCRIPTION:TEST\n" +
                   "END:VALARM",
                   Ci.calIAlarm.ALARM_RELATED_START);

    // trigger set 15 minutes prior to the start of the event
    check_relative("BEGIN:VALARM\n" +
                   "ACTION:DISPLAY\n" +
                   "TRIGGER;VALUE=DURATION;RELATED=START:-PT15M\n" +
                   "DESCRIPTION:TEST\n" +
                   "END:VALARM",
                   Ci.calIAlarm.ALARM_RELATED_START);

    // trigger set 5 minutes after the end of an event
    check_relative("BEGIN:VALARM\n" +
                   "ACTION:DISPLAY\n" +
                   "TRIGGER;RELATED=END:PT5M\n" +
                   "DESCRIPTION:TEST\n" +
                   "END:VALARM",
                   Ci.calIAlarm.ALARM_RELATED_END);

    // trigger set 5 minutes after the end of an event
    check_relative("BEGIN:VALARM\n" +
                   "ACTION:DISPLAY\n" +
                   "TRIGGER;VALUE=DURATION;RELATED=END:PT5M\n" +
                   "DESCRIPTION:TEST\n" +
                   "END:VALARM",
                   Ci.calIAlarm.ALARM_RELATED_END);

    // trigger set to an absolute date/time
    check_absolute("BEGIN:VALARM\n" +
                   "ACTION:DISPLAY\n" +
                   "TRIGGER;VALUE=DATE-TIME:20090430T080000Z\n" +
                   "DESCRIPTION:TEST\n" +
                   "END:VALARM");
}

function check_relative(aIcalString, aRelated) {
    let alarm = cal.createAlarm();
    alarm.icalString = aIcalString;
    do_check_eq(alarm.related, aRelated);
    do_check_eq(alarm.alarmDate, null);
    do_check_neq(alarm.offset, null);
}

function check_absolute(aIcalString) {
    let alarm = cal.createAlarm();
    alarm.icalString = aIcalString;
    do_check_eq(alarm.related, Ci.calIAlarm.ALARM_RELATED_ABSOLUTE);
    do_check_neq(alarm.alarmDate, null);
    do_check_eq(alarm.offset, null);
}
