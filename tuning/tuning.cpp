// tuning.cpp
/* Contains:
 * setScaleSingleValue
 * get_scales
 * Tuning_BulkUpdate
 * Tuning_NoteUpdate
 * Tuning_QueryTemp
 * Tuning_BaseKey_select
 */

#include	"JVlibForm.h"
#include	<QtGui>

QByteArray JVlibForm::Tuning_currentTuning = 0;
static QByteArray Cents('\0');

void JVlibForm::setScaleSingleValue(int addr, int val) {
  unsigned char buf[5];
  if (!state_table->jv_connect || !state_table->updates_enabled) return;
  memset(buf,0,sizeof(buf));
  if (state_table->patch_mode)
    buf[2] = 0x20;
  else
    buf[2] = 0x10 + Tuning_PartTune_select->value() - 1;
  buf[3] = addr;
  buf[4] = val;
  if (open_ports() == EXIT_FAILURE) return;
  if (sysex_update(&buf[0], sizeof(buf)) == EXIT_FAILURE) {
    puts("tuning udpate failed");
    close_ports();
    return;
  }
  close_ports();
  state_table->tuning_modified = true;
}	// end setScaleSingleValue

void JVlibForm::Tuning_BulkUpdate(int pn, int offset, int val) {
  // synchronize parts so the given note(offset) is the same val in every enabled part
  system_area->sys_part_scale_tune[pn-1].scale[offset] = val;
  Tuning_PartTune_select->blockSignals(true);
  Tuning_PartTune_select->setValue(pn);	// set this part number for use by setScaleSingleValue
  Tuning_PartTune_select->blockSignals(false);
  if (state_table->jv_connect)
    setScaleSingleValue(offset,val);  
}	// end Tuning_BulkUpdate

void JVlibForm::Tuning_NoteUpdate(int offset, int val) {
  // update local memory and JV with new Note(offset) value(val) for the specified individual Part displayed in the Part Tuning box
  if (!state_table->updates_enabled) return;
    if (state_table->patch_mode) {
      system_area->sys_patch_scale_tune.scale[offset] = val;
    }
    if (state_table->perf_mode) {
      system_area->sys_part_scale_tune[Tuning_PartTune_select->value()-1].scale[offset] = val;
    }
    if (state_table->jv_connect)
      setScaleSingleValue(offset,val);
    state_table->tuning_modified = true;
}	// end Tuning_NoteUpdate

void JVlibForm::Tuning_QueryTemp(int val) {
  static char Temp[12];
  switch(val) {
    case 0:	// Equal
      Cents.fill(0x40, 12);
      break;
    case 1:	// Just
      Temp = { 0x40, 0x4C, 0x44, 0x50, 0x32, 0x3E, 0x2E, 0x42, 0x24, 0x30, 0x52, 0x34 };
      Cents = QByteArray::fromRawData(Temp, 12);
      break;
    case 2:	// Pythagorean
      Temp = {  0x40, 0x36, 0x44, 0x3A, 0x48, 0x3E, 0x4C, 0x42, 0x38, 0x46, 0x3C, 0x4A};
      Cents = QByteArray::fromRawData(Temp, 12);
      break;
    case 3:	// Pure
      Temp = { 0x40, 0x38, 0x44, 0x50, 0x32, 0x3E, 0x36, 0x42, 0x4E, 0x30, 0x4E, 0x34 };
      Cents = QByteArray::fromRawData(Temp, 12);
      break;
    case 4:	// Well
      Temp = { 0x46, 0x40, 0x42, 0x44, 0x3E, 0x46, 0x3E, 0x44, 0x42, 0x40, 0x46, 0x3E };
      Cents = QByteArray::fromRawData(Temp, 12);
      break;
    case 5:	// Meantone
      Temp = { 0x40, 0x28, 0x39, 0x4B, 0x32, 0x44, 0x2B, 0x3D, 0x24, 0x36, 0x47, 0x2E };
      Cents = QByteArray::fromRawData(Temp, 12);
      break;
    case 6:	// Arabic
      Temp = { 0x3A, 0x6D, 0x3E, 0x34, 0x0D, 0x38, 0x6B, 0x3C, 0x6F, 0x40, 0x36, 0x0F };
      Cents = QByteArray::fromRawData(Temp, 12);
      break;
    default:
      return;
      break;
  }	// end switch
  // fill Tuning_currentTuning with new data
  if (state_table->patch_mode) {
    Tuning_BaseKey_select(0);
  }
  if (state_table->perf_mode) {
    // update each part with the new Temperament settings, initially in C Major
    for (int offset=0;offset<12;offset++) {
	if (Tuning_Part1Tuning_select->isChecked()) Tuning_BulkUpdate(1, offset, Cents.at(offset));
	if (Tuning_Part2Tuning_select->isChecked()) Tuning_BulkUpdate(2, offset, Cents.at(offset));
	if (Tuning_Part3Tuning_select->isChecked()) Tuning_BulkUpdate(3, offset, Cents.at(offset));
	if (Tuning_Part4Tuning_select->isChecked()) Tuning_BulkUpdate(4, offset, Cents.at(offset));
	if (Tuning_Part5Tuning_select->isChecked()) Tuning_BulkUpdate(5, offset, Cents.at(offset));
	if (Tuning_Part6Tuning_select->isChecked()) Tuning_BulkUpdate(6, offset, Cents.at(offset));
	if (Tuning_Part7Tuning_select->isChecked()) Tuning_BulkUpdate(7, offset, Cents.at(offset));
	if (Tuning_Part8Tuning_select->isChecked()) Tuning_BulkUpdate(8, offset, Cents.at(offset));
	if (Tuning_Part9Tuning_select->isChecked()) Tuning_BulkUpdate(9, offset, Cents.at(offset));
	if (Tuning_Part10Tuning_select->isChecked()) Tuning_BulkUpdate(10, offset, Cents.at(offset));
	if (Tuning_Part11Tuning_select->isChecked()) Tuning_BulkUpdate(11, offset, Cents.at(offset));
	if (Tuning_Part12Tuning_select->isChecked()) Tuning_BulkUpdate(12, offset, Cents.at(offset));
	if (Tuning_Part13Tuning_select->isChecked()) Tuning_BulkUpdate(13, offset, Cents.at(offset));
	if (Tuning_Part14Tuning_select->isChecked()) Tuning_BulkUpdate(14, offset, Cents.at(offset));
	if (Tuning_Part15Tuning_select->isChecked()) Tuning_BulkUpdate(15, offset, Cents.at(offset));
	if (Tuning_Part16Tuning_select->isChecked()) Tuning_BulkUpdate(16, offset, Cents.at(offset));
    }	// end FOR
    Tuning_PartTune_select->setValue(1);
  }	// end IF perf mode
}	// end Tuning_QueryTemp

void JVlibForm::Tuning_BaseKey_select(int val) {
  int x=val;
  // set Display to adjusted value based on the first 12 entries in Cents
  Tuning_PartTuneC_select->setValue(Cents.at(x)-64); x=(x<11 ? x+1 : 0);
  Tuning_PartTuneCs_select->setValue(Cents.at(x)-64); x=(x<11 ? x+1 : 0);
  Tuning_PartTuneD_select->setValue(Cents.at(x)-64); x=(x<11 ? x+1 : 0);
  Tuning_PartTuneDs_select->setValue(Cents.at(x)-64); x=(x<11 ? x+1 : 0);
  Tuning_PartTuneE_select->setValue(Cents.at(x)-64); x=(x<11 ? x+1 : 0);
  Tuning_PartTuneF_select->setValue(Cents.at(x)-64); x=(x<11 ? x+1 : 0);
  Tuning_PartTuneFs_select->setValue(Cents.at(x)-64); x=(x<11 ? x+1 : 0);
  Tuning_PartTuneG_select->setValue(Cents.at(x)-64); x=(x<11 ? x+1 : 0);
  Tuning_PartTuneGs_select->setValue(Cents.at(x)-64); x=(x<11 ? x+1 : 0);
  Tuning_PartTuneA_select->setValue(Cents.at(x)-64); x=(x<11 ? x+1 : 0);
  Tuning_PartTuneAs_select->setValue(Cents.at(x)-64); x=(x<11 ? x+1 : 0);
  Tuning_PartTuneB_select->setValue(Cents.at(x)-64);
}	// end Tuning_BaseKey_select

void JVlibForm::Tuning_BaseKey_set(int val) {
  if (state_table->patch_mode) {
    Tuning_BaseKey_select(val);
  }
  if (state_table->perf_mode) {
    if (Tuning_Part1Tuning_select->isChecked()) { Tuning_PartTune_select->setValue(1); Tuning_BaseKey_select(val); }
    if (Tuning_Part2Tuning_select->isChecked()) { Tuning_PartTune_select->setValue(2); Tuning_BaseKey_select(val); }
    if (Tuning_Part3Tuning_select->isChecked()) { Tuning_PartTune_select->setValue(3); Tuning_BaseKey_select(val); }
    if (Tuning_Part4Tuning_select->isChecked()) { Tuning_PartTune_select->setValue(4); Tuning_BaseKey_select(val); }
    if (Tuning_Part5Tuning_select->isChecked()) { Tuning_PartTune_select->setValue(5); Tuning_BaseKey_select(val); }
    if (Tuning_Part6Tuning_select->isChecked()) { Tuning_PartTune_select->setValue(6); Tuning_BaseKey_select(val); }
    if (Tuning_Part7Tuning_select->isChecked()) { Tuning_PartTune_select->setValue(7); Tuning_BaseKey_select(val); }
    if (Tuning_Part8Tuning_select->isChecked()) { Tuning_PartTune_select->setValue(8); Tuning_BaseKey_select(val); }
    if (Tuning_Part9Tuning_select->isChecked()) { Tuning_PartTune_select->setValue(9); Tuning_BaseKey_select(val); }
    if (Tuning_Part10Tuning_select->isChecked()) { Tuning_PartTune_select->setValue(10); Tuning_BaseKey_select(val); }
    if (Tuning_Part11Tuning_select->isChecked()) { Tuning_PartTune_select->setValue(11); Tuning_BaseKey_select(val); }
    if (Tuning_Part12Tuning_select->isChecked()) { Tuning_PartTune_select->setValue(12); Tuning_BaseKey_select(val); }
    if (Tuning_Part13Tuning_select->isChecked()) { Tuning_PartTune_select->setValue(13); Tuning_BaseKey_select(val); }
    if (Tuning_Part14Tuning_select->isChecked()) { Tuning_PartTune_select->setValue(14); Tuning_BaseKey_select(val); }
    if (Tuning_Part15Tuning_select->isChecked()) { Tuning_PartTune_select->setValue(15); Tuning_BaseKey_select(val); }
    if (Tuning_Part16Tuning_select->isChecked()) { Tuning_PartTune_select->setValue(16); Tuning_BaseKey_select(val); }
  }
}
