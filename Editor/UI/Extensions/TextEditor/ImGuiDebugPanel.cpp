#if _DEBUG

#include "TextEditor.h"

void TextEditor::ImGuiDebugPanel(const std::string& panelName)
{
	ImGui::Begin(panelName.c_str());

	if (ImGui::CollapsingHeader("Cursor info"))
	{
		ImGui::DragInt("Cursor count", &mState.mCurrentCursor);
		for (int i = 0; i <= mState.mCurrentCursor; i++)
		{
			ImGui::DragInt2("Cursor", &mState.mCursors[i].mCursorPosition.mLine);
			ImGui::DragInt2("Selection start", &mState.mCursors[i].mSelectionStart.mLine);
			ImGui::DragInt2("Selection end", &mState.mCursors[i].mSelectionEnd.mLine);
			ImGui::DragInt2("Interactive start", &mState.mCursors[i].mInteractiveStart.mLine);
			ImGui::DragInt2("Interactive end", &mState.mCursors[i].mInteractiveEnd.mLine);
		}
	}
	if (ImGui::CollapsingHeader("Undo"))
	{
		static std::string numberOfRecordsText;
		numberOfRecordsText = "Number of records: " + std::to_string(mUndoBuffer.size());
		ImGui::Text(numberOfRecordsText.c_str());
		ImGui::DragInt("Undo index", &mState.mCurrentCursor);
		for (int i = 0; i < mUndoBuffer.size(); i++)
		{
			if (ImGui::CollapsingHeader(std::to_string(i).c_str()))
			{

				ImGui::Text("Operations");
				for (int j = 0; j < mUndoBuffer[i].mOperations.size(); j++)
				{
					ImGui::Text(mUndoBuffer[i].mOperations[j].mText.c_str());
					ImGui::Text(mUndoBuffer[i].mOperations[j].mType == UndoOperationType::Add ? "Add" : "Delete");
					ImGui::DragInt2("Start", &mUndoBuffer[i].mOperations[j].mStart.mLine);
					ImGui::DragInt2("End", &mUndoBuffer[i].mOperations[j].mEnd.mLine);
					ImGui::Separator();
				}
			}
		}
	}
	if (ImGui::Button("Run unit tests"))
	{
		UnitTests();
	}
	ImGui::End();
}

void TextEditor::UnitTests() {
	// --- GetCharacterColumn --- //
	SetText(" \t  \t   \t \t\n");
	assert(GetCharacterColumn(0, 0) == 0);
	assert(GetCharacterColumn(0, 1) == 1);
	assert(GetCharacterColumn(0, 2) == 4);
	assert(GetCharacterColumn(0, 3) == 5);
	assert(GetCharacterColumn(0, 4) == 6);
	assert(GetCharacterColumn(0, 5) == 8);
	assert(GetCharacterColumn(0, 6) == 9);
	assert(GetCharacterColumn(0, 7) == 10);
	assert(GetCharacterColumn(0, 8) == 11);
	assert(GetCharacterColumn(0, 9) == 12);
	assert(GetCharacterColumn(0, 10) == 13);
	assert(GetCharacterColumn(0, 11) == 16);
	assert(GetCharacterColumn(0, 12) == 16); // out of range
	// empty line
	assert(GetCharacterColumn(1, 0) == 0);
	assert(GetCharacterColumn(1, 1) == 0); // out of range
	assert(GetCharacterColumn(1, 2) == 0); // out of range
	// nonexistent line
	assert(GetCharacterColumn(2, 0) == 0);
	assert(GetCharacterColumn(2, 1) == 0);
	assert(GetCharacterColumn(2, 2) == 0);

	// --- GetCharacterIndexLeftSide --- //
	assert(GetCharacterIndexLeftSide({ 0,0 }) == 0);
	assert(GetCharacterIndexLeftSide({ 0,1 }) == 1);
	assert(GetCharacterIndexLeftSide({ 0,2 }) == 1);
	assert(GetCharacterIndexLeftSide({ 0,3 }) == 1);
	assert(GetCharacterIndexLeftSide({ 0,4 }) == 2);
	assert(GetCharacterIndexLeftSide({ 0,5 }) == 3);
	assert(GetCharacterIndexLeftSide({ 0,6 }) == 4);
	assert(GetCharacterIndexLeftSide({ 0,7 }) == 4);
	assert(GetCharacterIndexLeftSide({ 0,8 }) == 5);
	assert(GetCharacterIndexLeftSide({ 0,9 }) == 6);
	assert(GetCharacterIndexLeftSide({ 0,10 }) == 7);
	assert(GetCharacterIndexLeftSide({ 0,11 }) == 8);
	assert(GetCharacterIndexLeftSide({ 0,12 }) == 9);
	assert(GetCharacterIndexLeftSide({ 0,13 }) == 10);
	assert(GetCharacterIndexLeftSide({ 0,14 }) == 10);
	assert(GetCharacterIndexLeftSide({ 0,15 }) == 10);
	assert(GetCharacterIndexLeftSide({ 0,16 }) == 11);
	assert(GetCharacterIndexLeftSide({ 0,17 }) == 11); // out of range
	assert(GetCharacterIndexLeftSide({ 0,18 }) == 11); // out of range
	// empty line
	assert(GetCharacterIndexLeftSide({ 1,0 }) == 0);
	assert(GetCharacterIndexLeftSide({ 1,1 }) == 0); // out of range
	assert(GetCharacterIndexLeftSide({ 1,2 }) == 0); // out of range
	// nonexistent line
	assert(GetCharacterIndexLeftSide({ 2,0 }) == -1);
	assert(GetCharacterIndexLeftSide({ 2,1 }) == -1);
	assert(GetCharacterIndexLeftSide({ 2,2 }) == -1);

	// --- Advance --- //
	Coordinates testCoords = { 0,0 };
	Advance(testCoords);
	assert(testCoords.mLine == 0 && testCoords.mColumn == 1);
	Advance(testCoords);
	assert(testCoords.mLine == 0 && testCoords.mColumn == 4);
	Advance(testCoords);
	assert(testCoords.mLine == 0 && testCoords.mColumn == 5);
	Advance(testCoords);
	assert(testCoords.mLine == 0 && testCoords.mColumn == 6);
	Advance(testCoords);
	assert(testCoords.mLine == 0 && testCoords.mColumn == 8);
	Advance(testCoords);
	assert(testCoords.mLine == 0 && testCoords.mColumn == 9);
	Advance(testCoords);
	assert(testCoords.mLine == 0 && testCoords.mColumn == 10);
	Advance(testCoords);
	assert(testCoords.mLine == 0 && testCoords.mColumn == 11);
	Advance(testCoords);
	assert(testCoords.mLine == 0 && testCoords.mColumn == 12);
	Advance(testCoords);
	assert(testCoords.mLine == 0 && testCoords.mColumn == 13);
	Advance(testCoords);
	assert(testCoords.mLine == 0 && testCoords.mColumn == 16);
	Advance(testCoords);
	assert(testCoords.mLine == 1 && testCoords.mColumn == 0);
	Advance(testCoords);
	assert(testCoords.mLine == 1 && testCoords.mColumn == 0);
}
#endif