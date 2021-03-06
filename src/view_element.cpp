/////////////////////////////////////////////////////////////////////////////
// Name:        view_element.cpp
// Author:      Laurent Pugin and Chris Niven
// Created:     2011
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "view.h"

//----------------------------------------------------------------------------

#include <assert.h>
#include <iostream>
#include <math.h>

//----------------------------------------------------------------------------

#include "accid.h"
#include "artic.h"
#include "beam.h"
#include "chord.h"
#include "clef.h"
#include "custos.h"
#include "devicecontext.h"
#include "doc.h"
#include "dot.h"
#include "dynam.h"
#include "elementpart.h"
#include "functorparams.h"
#include "keysig.h"
#include "layer.h"
#include "measure.h"
#include "mensur.h"
#include "metersig.h"
#include "mrest.h"
#include "multirest.h"
#include "note.h"
#include "proport.h"
#include "rest.h"
#include "rpt.h"
#include "smufl.h"
#include "space.h"
#include "staff.h"
#include "style.h"
#include "syl.h"
#include "system.h"
#include "tie.h"
#include "tuplet.h"
#include "verse.h"
#include "vrv.h"

namespace vrv {

void View::DrawLayerElement(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    int previousColor = m_currentColour;

    if (element == m_currentElement) {
        m_currentColour = AxRED;
    }
    else {
        m_currentColour = AxBLACK;
    }

    if (element->Is(ACCID)) {
        DrawAccid(dc, element, layer, staff, measure);
    }
    else if (element->Is(ARTIC)) {
        DrawArtic(dc, element, layer, staff, measure);
    }
    else if (element->Is(ARTIC_PART)) {
        DrawArticPart(dc, element, layer, staff, measure);
    }
    else if (element->Is(BARLINE)) {
        DrawBarLine(dc, element, layer, staff, measure);
    }
    else if (element->Is(BEAM)) {
        DrawBeam(dc, element, layer, staff, measure);
    }
    else if (element->Is(BEATRPT)) {
        DrawBeatRpt(dc, element, layer, staff, measure);
    }
    else if (element->Is(BTREM)) {
        DrawBTrem(dc, element, layer, staff, measure);
    }
    else if (element->Is(CHORD)) {
        DrawDurationElement(dc, element, layer, staff, measure);
    }
    else if (element->Is(CLEF)) {
        DrawClef(dc, element, layer, staff, measure);
    }
    else if (element->Is(CUSTOS)) {
        DrawCustos(dc, element, layer, staff, measure);
    }
    else if (element->Is(DOT)) {
        DrawDot(dc, element, layer, staff, measure);
    }
    else if (element->Is(DOTS)) {
        DrawDots(dc, element, layer, staff, measure);
    }
    else if (element->Is(FTREM)) {
        DrawFTrem(dc, element, layer, staff, measure);
    }
    else if (element->Is(FLAG)) {
        DrawFlag(dc, element, layer, staff, measure);
    }
    else if (element->Is(KEYSIG)) {
        DrawKeySig(dc, element, layer, staff, measure);
    }
    else if (element->Is(LIGATURE)) {
        DrawLigature(dc, element, layer, staff, measure);
    }
    else if (element->Is(MENSUR)) {
        DrawMensur(dc, element, layer, staff, measure);
    }
    else if (element->Is(METERSIG)) {
        DrawMeterSig(dc, element, layer, staff, measure);
    }
    else if (element->Is(MREST)) {
        DrawMRest(dc, element, layer, staff, measure);
    }
    else if (element->Is(MRPT)) {
        DrawMRpt(dc, element, layer, staff, measure);
    }
    else if (element->Is(MRPT2)) {
        DrawMRpt2(dc, element, layer, staff, measure);
    }
    else if (element->Is(MULTIREST)) {
        DrawMultiRest(dc, element, layer, staff, measure);
    }
    else if (element->Is(MULTIRPT)) {
        DrawMultiRpt(dc, element, layer, staff, measure);
    }
    else if (element->Is(NOTE)) {
        DrawDurationElement(dc, element, layer, staff, measure);
    }
    else if (element->Is(PROPORT)) {
        DrawProport(dc, element, layer, staff, measure);
    }
    else if (element->Is(REST)) {
        DrawDurationElement(dc, element, layer, staff, measure);
    }
    else if (element->Is(SPACE)) {
        DrawSpace(dc, element, layer, staff, measure);
    }
    else if (element->Is(STEM)) {
        DrawStem(dc, element, layer, staff, measure);
    }
    else if (element->Is(SYL)) {
        DrawSyl(dc, element, layer, staff, measure);
    }
    else if (element->Is(TUPLET)) {
        DrawTuplet(dc, element, layer, staff, measure);
    }
    else if (element->Is(VERSE)) {
        DrawVerse(dc, element, layer, staff, measure);
    }
    else {
        // This should never happen
        LogError("Element '%s' cannot be drawn", element->GetClassName().c_str());
    }

    m_currentColour = previousColor;
}

//----------------------------------------------------------------------------
// View - LayerElement
//----------------------------------------------------------------------------

void View::DrawAccid(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    Accid *accid = dynamic_cast<Accid *>(element);
    assert(accid);

    // This can happen with accid within note with only accid.ges
    if (!accid->HasAccid()) {
        accid->SetEmptyBB();
        return;
    }

    dc->StartGraphic(element, "", element->GetUuid());

    /************** editorial accidental **************/

    std::wstring accidStr = accid->GetSymbolStr();

    int x = accid->GetDrawingX();
    int y = accid->GetDrawingY();

    if (accid->GetFunc() == accidLog_FUNC_edit) {
        y = staff->GetDrawingY();
        // look at the note position and adjust it if necessary
        Note *note = dynamic_cast<Note *>(accid->GetFirstParent(NOTE, MAX_ACCID_DEPTH));
        if (note) {
            // Check if the note is on the top line or above (add a unit for the note head half size)
            if (note->GetDrawingY() >= y) y = note->GetDrawingY() + m_doc->GetDrawingUnit(staff->m_drawingStaffSize);
            // Check if the top of the stem is above
            if ((note->GetDrawingStemDir() == STEMDIRECTION_up) && (note->GetDrawingStemEnd(note).y > y))
                y = note->GetDrawingStemEnd(note).y;
        }
        TextExtend extend;
        dc->SetFont(m_doc->GetDrawingSmuflFont(staff->m_drawingStaffSize, accid->IsCueSize()));
        dc->GetSmuflTextExtent(accid->GetSymbolStr(), &extend);
        dc->ResetFont();
        y += extend.m_descent + m_doc->GetDrawingUnit(staff->m_drawingStaffSize);
    }

    DrawSmuflString(dc, x, y, accidStr, true, staff->m_drawingStaffSize, accid->IsCueSize(), true);

    dc->EndGraphic(element, this);
}

void View::DrawArtic(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    Artic *artic = dynamic_cast<Artic *>(element);
    assert(artic);

    /************** draw the artic **************/

    dc->StartGraphic(element, "", element->GetUuid());

    DrawLayerChildren(dc, artic, layer, staff, measure);

    dc->EndGraphic(element, this);
}

void View::DrawArticPart(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    ArticPart *articPart = dynamic_cast<ArticPart *>(element);
    assert(articPart);

    /************** draw the artic **************/

    wchar_t code;

    int x = articPart->GetDrawingX();
    // HARDCODED value, we double the default margin for now - should go in styling
    int yShift = 2 * m_doc->GetTopMargin(articPart->GetClassId()) * m_doc->GetDrawingUnit(staff->m_drawingStaffSize)
        / PARAM_DENOMINATOR;
    int direction = (articPart->GetPlace() == STAFFREL_above) ? 1 : -1;

    int y = articPart->GetDrawingY();

    int xCorr;
    int baselineCorr;

    bool drawingCueSize = true;

    dc->StartGraphic(element, "", element->GetUuid());

    dc->SetFont(m_doc->GetDrawingSmuflFont(staff->m_drawingStaffSize, drawingCueSize));

    std::vector<data_ARTICULATION>::iterator articIter;
    std::vector<data_ARTICULATION> articList = articPart->GetArtic();
    for (articIter = articList.begin(); articIter != articList.end(); articIter++) {

        code = Artic::GetSmuflCode(*articIter, articPart->GetPlace());

        // Skip it if we do not have it in the font (for now - we should log / document this somewhere)
        if (code == 0) {
            articPart->SetEmptyBB(true);
            continue;
        }

        if (articPart->GetType() == ARTIC_PART_INSIDE) {
            // If we are above the top of the  staff, just pile them up
            if ((articPart->GetPlace() == STAFFREL_above) && (y > staff->GetDrawingY())) y += yShift;
            // If we are below the bottom, just pile the down
            else if ((articPart->GetPlace() == STAFFREL_below)
                && (y < staff->GetDrawingY() - m_doc->GetDrawingStaffSize(staff->m_drawingStaffSize)))
                y -= yShift;
            // Otherwise make it fit the staff space
            else {
                y = GetNearestInterStaffPosition(y, staff, articPart->GetPlace());
                if (IsOnStaffLine(y, staff)) y += m_doc->GetDrawingUnit(staff->m_drawingStaffSize) * direction;
            }
        }
        // Artic part outside just need to be piled up or down
        else
            y += yShift * direction;

        // The correction for centering the glyph
        xCorr = m_doc->GetGlyphWidth(code, staff->m_drawingStaffSize, drawingCueSize) / 2;
        // The position of the next glyph (and for correcting the baseline if necessary
        int glyphHeight = m_doc->GetGlyphHeight(code, staff->m_drawingStaffSize, drawingCueSize);

        // Center the glyh if necessary
        if (Artic::IsCentered(*articIter)) {
            y += (articPart->GetPlace() == STAFFREL_above) ? -(glyphHeight / 2) : (glyphHeight / 2);
        }

        // Adjust the baseline for glyph above the baseline in SMuFL
        baselineCorr = 0;
        if (Artic::VerticalCorr(code, articPart->GetPlace())) baselineCorr = glyphHeight;

        DrawSmuflCode(dc, x - xCorr, y - baselineCorr, code, staff->m_drawingStaffSize, drawingCueSize);

        // Adjusting the y position for the next artic
        y += glyphHeight * direction;
    }

    dc->EndGraphic(element, this);
}

void View::DrawBarLine(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    BarLine *barLine = dynamic_cast<BarLine *>(element);
    assert(barLine);

    if (barLine->GetForm() == BARRENDITION_invis) {
        barLine->SetEmptyBB();
        return;
    }

    dc->StartGraphic(element, "", element->GetUuid());

    int y = staff->GetDrawingY();
    DrawBarLine(dc, y, y - m_doc->GetDrawingStaffSize(staff->m_drawingStaffSize), barLine);

    dc->EndGraphic(element, this);
}

void View::DrawBeatRpt(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    BeatRpt *beatRpt = dynamic_cast<BeatRpt *>(element);
    assert(beatRpt);

    dc->StartGraphic(element, "", element->GetUuid());

    int x = element->GetDrawingX();
    int xSymbol = x;
    int y = element->GetDrawingY();
    y -= staff->m_drawingLines / 2 * m_doc->GetDrawingDoubleUnit(staff->m_drawingStaffSize);

    if (beatRpt->GetForm() == BEATRPT_REND_mixed) {
        DrawSmuflCode(dc, xSymbol, y, SMUFL_E501_repeat2Bars, staff->m_drawingStaffSize, false);
    }
    else {
        DrawSmuflCode(dc, xSymbol, y, SMUFL_E101_noteheadSlashHorizontalEnds, staff->m_drawingStaffSize, false);
        int additionalSlash = beatRpt->GetForm() - BEATRPT_REND_8;
        int halfWidth
            = m_doc->GetGlyphWidth(SMUFL_E101_noteheadSlashHorizontalEnds, staff->m_drawingStaffSize, false) / 2;
        int i;
        for (i = 0; i < additionalSlash; i++) {
            xSymbol += halfWidth;
            DrawSmuflCode(dc, xSymbol, y, SMUFL_E101_noteheadSlashHorizontalEnds, staff->m_drawingStaffSize, false);
        }
    }

    dc->EndGraphic(element, this);
}

void View::DrawBTrem(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    BTrem *bTrem = dynamic_cast<BTrem *>(element);
    assert(bTrem);

    data_STEMDIRECTION stemDir = STEMDIRECTION_NONE;
    data_STEMMODIFIER stemMod;
    int drawingDur;
    LayerElement *childElement = NULL;
    Note *childNote = NULL;
    Chord *childChord = NULL;
    Point stemPoint;
    bool drawingCueSize = false;
    int x, y;

    childChord = dynamic_cast<Chord *>(bTrem->FindChildByType(CHORD));
    // Get from the chord or note child
    if (childChord) {
        drawingDur = childChord->GetDur();
        childElement = childChord;
    }
    else {
        childNote = dynamic_cast<Note *>(bTrem->FindChildByType(NOTE));
        if (childNote) {
            drawingDur = childNote->GetDur();
            childElement = childNote;
        }
    }

    if (!childElement) {
        bTrem->SetEmptyBB();
        return;
    }

    dc->StartGraphic(element, "", element->GetUuid());

    DrawLayerChildren(dc, bTrem, layer, staff, measure);

    // Get stem values from the chord or note child
    if (childChord) {
        Stem *stem = childChord->GetDrawingStem();
        stemDir = childChord->GetDrawingStemDir();
        stemMod = stem ? stem->GetStemMod() : STEMMODIFIER_NONE;
        stemPoint = childChord->GetDrawingStemStart(childChord);
    }
    else {
        Stem *stem = childNote->GetDrawingStem();
        drawingCueSize = childNote->IsCueSize();
        stemDir = childNote->GetDrawingStemDir();
        stemMod = stem ? stem->GetStemMod() : STEMMODIFIER_NONE;
        stemPoint = childNote->GetDrawingStemStart(childNote);
    }

    if (bTrem->HasMeasperf()) {
        switch (bTrem->GetMeasperf()) {
            case (DUR_8): stemMod = STEMMODIFIER_1slash; break;
            case (DUR_16): stemMod = STEMMODIFIER_2slash; break;
            case (DUR_32): stemMod = STEMMODIFIER_3slash; break;
            case (DUR_64): stemMod = STEMMODIFIER_4slash; break;
            case (DUR_128): stemMod = STEMMODIFIER_5slash; break;
            case (DUR_256): stemMod = STEMMODIFIER_6slash; break;
            default: break;
        }
    }

    int beamWidthBlack = m_doc->GetDrawingBeamWidth(staff->m_drawingStaffSize, drawingCueSize);
    int beamWidthWhite = m_doc->GetDrawingBeamWhiteWidth(staff->m_drawingStaffSize, drawingCueSize);
    int width = m_doc->GetGlyphWidth(SMUFL_E0A3_noteheadHalf, staff->m_drawingStaffSize, drawingCueSize);
    int height = beamWidthBlack * 7 / 10;
    int step = height + beamWidthWhite;

    if (stemDir == STEMDIRECTION_up) {
        if (drawingDur > DUR_1) {
            // Since we are adding the slashing on the stem, ignore artic
            y = childElement->GetDrawingTop(m_doc, staff->m_drawingStaffSize, false) - 3 * height;
            x = stemPoint.x;
        }
        else {
            // Take into account artic (not likely, though)
            y = childElement->GetDrawingTop(m_doc, staff->m_drawingStaffSize)
                + m_doc->GetDrawingUnit(staff->m_drawingStaffSize) * 3;
            x = childElement->GetDrawingX();
        }
        step = -step;
    }
    else {
        if (drawingDur > DUR_1) {
            // Idem as above
            y = childElement->GetDrawingBottom(m_doc, staff->m_drawingStaffSize, false) + 1 * height;
            x = stemPoint.x + m_doc->GetDrawingStemWidth(staff->m_drawingStaffSize);
        }
        else {
            y = childElement->GetDrawingBottom(m_doc, staff->m_drawingStaffSize)
                - m_doc->GetDrawingUnit(staff->m_drawingStaffSize) * 5;
            x = childElement->GetDrawingX();
        }
    }

    Beam *beam = childElement->IsInBeam();
    if (beam) {
        int beamStep = (drawingDur - DUR_8) * (beamWidthBlack + beamWidthWhite) + beamWidthWhite;
        y += (stemDir == STEMDIRECTION_down) ? beamStep : -beamStep;
    }

    int s;
    // by default draw 3 slashes (e.g., for a temolo on a whole note)
    if (stemMod == STEMMODIFIER_NONE) stemMod = STEMMODIFIER_3slash;
    for (s = 1; s < stemMod; s++) {
        DrawObliquePolygon(dc, x - width / 2, y, x + width / 2, y + height, height);
        y += step;
    }

    dc->EndGraphic(element, this);
}

void View::DrawChord(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    Chord *chord = dynamic_cast<Chord *>(element);
    assert(chord);

    if (chord->m_crossStaff) staff = chord->m_crossStaff;

    // For cross staff chords we need to re-calculate the stem because the staff position might have changed
    if (chord->HasCrossStaff()) {
        SetAlignmentPitchPosParams setAlignmentPitchPosParams(this->m_doc);
        Functor setAlignmentPitchPos(&Object::SetAlignmentPitchPos);
        chord->Process(&setAlignmentPitchPos, &setAlignmentPitchPosParams);

        CalcStemParams calcStemParams(this->m_doc);
        Functor calcStem(&Object::CalcStem);
        chord->Process(&calcStem, &calcStemParams);
    }

    chord->ResetDrawingList();

    /************ Draw children (notes, accidentals, etc) ************/

    DrawLayerChildren(dc, chord, layer, staff, measure);

    /************ Fermata attribute ************/

    if (chord->HasFermata()) {
        DrawFermataAttr(dc, element, layer, staff);
    }
}

void View::DrawClef(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    Clef *clef = dynamic_cast<Clef *>(element);
    assert(clef);

    int y = staff->GetDrawingY();
    int x = element->GetDrawingX();
    int sym = 0;
    bool isMensural = (staff->m_drawingNotationType == NOTATIONTYPE_mensural
        || staff->m_drawingNotationType == NOTATIONTYPE_mensural_white
        || staff->m_drawingNotationType == NOTATIONTYPE_mensural_black);

    int shapeOctaveDis = Clef::ClefId(clef->GetShape(), 0, clef->GetDis(), clef->GetDisPlace());

    if (shapeOctaveDis == Clef::ClefId(CLEFSHAPE_G, 0, OCTAVE_DIS_NONE, PLACE_NONE))
        sym = SMUFL_E050_gClef;
    else if (shapeOctaveDis == Clef::ClefId(CLEFSHAPE_G, 0, OCTAVE_DIS_8, PLACE_below))
        sym = SMUFL_E052_gClef8vb;
    else if (shapeOctaveDis == Clef::ClefId(CLEFSHAPE_G, 0, OCTAVE_DIS_15, PLACE_below))
        sym = SMUFL_E051_gClef15mb;
    else if (shapeOctaveDis == Clef::ClefId(CLEFSHAPE_G, 0, OCTAVE_DIS_8, PLACE_above))
        sym = SMUFL_E053_gClef8va;
    else if (shapeOctaveDis == Clef::ClefId(CLEFSHAPE_G, 0, OCTAVE_DIS_15, PLACE_above))
        sym = SMUFL_E054_gClef15ma;
    // C-clef
    else if (shapeOctaveDis == Clef::ClefId(CLEFSHAPE_C, 0, OCTAVE_DIS_NONE, PLACE_NONE))
        sym = SMUFL_E05C_cClef;
    else if (shapeOctaveDis == Clef::ClefId(CLEFSHAPE_C, 0, OCTAVE_DIS_8, PLACE_below))
        sym = SMUFL_E05D_cClef8vb;
    else if (clef->GetShape() == CLEFSHAPE_C)
        sym = SMUFL_E05C_cClef;

    // F-clef
    else if (shapeOctaveDis == Clef::ClefId(CLEFSHAPE_F, 0, OCTAVE_DIS_NONE, PLACE_NONE))
        sym = SMUFL_E062_fClef;
    else if (shapeOctaveDis == Clef::ClefId(CLEFSHAPE_F, 0, OCTAVE_DIS_8, PLACE_below))
        sym = SMUFL_E064_fClef8vb;
    else if (shapeOctaveDis == Clef::ClefId(CLEFSHAPE_F, 0, OCTAVE_DIS_15, PLACE_below))
        sym = SMUFL_E063_fClef15mb;
    else if (shapeOctaveDis == Clef::ClefId(CLEFSHAPE_F, 0, OCTAVE_DIS_8, PLACE_above))
        sym = SMUFL_E065_fClef8va;
    else if (shapeOctaveDis == Clef::ClefId(CLEFSHAPE_F, 0, OCTAVE_DIS_15, PLACE_above))
        sym = SMUFL_E066_fClef15ma;
    else if (clef->GetShape() == CLEFSHAPE_F)
        sym = SMUFL_E062_fClef;

    // Perc
    else if (clef->GetShape() == CLEFSHAPE_perc)
        sym = SMUFL_E069_unpitchedPercussionClef1;

    // mensural clefs
    if (isMensural) {
        if (staff->m_drawingNotationType == NOTATIONTYPE_mensural_black) {
            if (sym == SMUFL_E050_gClef)
                // G clef doesn't exist in black notation, so should never get here, but just in case.
                sym = SMUFL_E901_mensuralGclefPetrucci;
            else if (sym == SMUFL_E05C_cClef)
                sym = SMUFL_E906_chantCclef;
            else if (sym == SMUFL_E062_fClef)
                sym = SMUFL_E902_chantFclef;
        }
        else {
            if (sym == SMUFL_E050_gClef)
                sym = SMUFL_E901_mensuralGclefPetrucci;
            else if (sym == SMUFL_E05C_cClef)
                sym = SMUFL_E909_mensuralCclefPetrucciPosMiddle;
            else if (sym == SMUFL_E062_fClef)
                sym = SMUFL_E904_mensuralFclefPetrucci;
        }
    }

    if (sym == 0) {
        clef->SetEmptyBB();
        return;
    }

    y -= m_doc->GetDrawingDoubleUnit(staff->m_drawingStaffSize) * (staff->m_drawingLines - clef->GetLine());

    bool cueSize = false;
    if (clef->GetAlignment() && (clef->GetAlignment()->GetType() == ALIGNMENT_CLEF)) {
        if (m_doc->GetType() != Transcription) {
            cueSize = true;
            // HARDCODED
            x -= m_doc->GetGlyphWidth(sym, staff->m_drawingStaffSize, cueSize) * 1.35;
        }
    }

    dc->StartGraphic(element, "", element->GetUuid());

    DrawSmuflCode(dc, x, y, sym, staff->m_drawingStaffSize, cueSize);

    dc->EndGraphic(element, this);
}

void View::DrawCustos(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    // Custos *custos = dynamic_cast<Custos *>(element);
    // assert(custos);

    dc->StartGraphic(element, "", element->GetUuid());

    int x = element->GetDrawingX();
    int y = element->GetDrawingY();

    y -= m_doc->GetDrawingUnit(staff->m_drawingStaffSize) - m_doc->GetDrawingUnit(staff->m_drawingStaffSize) / 4;

    // HARDCODED (smufl code wrong)
    DrawSmuflCode(dc, x, y, 35, staff->m_drawingStaffSize, false);

    dc->EndGraphic(element, this);
}

void View::DrawDot(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    Dot *dot = dynamic_cast<Dot *>(element);
    assert(dot);

    dc->StartGraphic(element, "", element->GetUuid());

    int x = element->GetDrawingX();
    int y = element->GetDrawingY();

    // Use the note to which the points to for position
    if (dot->m_drawingNote && (m_doc->GetType() != Transcription)) {
        x = dot->m_drawingNote->GetDrawingX() + m_doc->GetDrawingUnit(staff->m_drawingStaffSize) * 7 / 2;
        y = dot->m_drawingNote->GetDrawingY();
    }

    DrawDotsPart(dc, x, y, 1, staff);

    dc->EndGraphic(element, this);
}

void View::DrawDots(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    Dots *dots = dynamic_cast<Dots *>(element);
    assert(dots);

    dc->StartGraphic(element, "", element->GetUuid());

    MapOfDotLocs::const_iterator iter;
    const MapOfDotLocs *map = dots->GetMapOfDotLocs();
    for (iter = map->begin(); iter != map->end(); iter++) {
        Staff *dotStaff = (iter->first) ? iter->first : staff;
        int y = dotStaff->GetDrawingY()
            - m_doc->GetDrawingDoubleUnit(staff->m_drawingStaffSize) * (dotStaff->m_drawingLines - 1);
        int x = dots->GetDrawingX() + m_doc->GetDrawingUnit(staff->m_drawingStaffSize);
        const std::list<int> *dotLocs = &iter->second;
        std::list<int>::const_iterator intIter;
        for (intIter = dotLocs->begin(); intIter != dotLocs->end(); intIter++) {
            DrawDotsPart(
                dc, x, y + (*intIter) * m_doc->GetDrawingUnit(staff->m_drawingStaffSize), dots->GetDots(), dotStaff);
        }
    }

    dc->EndGraphic(element, this);
}

void View::DrawDurationElement(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    if (dynamic_cast<Chord *>(element)) {
        dc->StartGraphic(element, "", element->GetUuid());
        DrawChord(dc, element, layer, staff, measure);
        dc->EndGraphic(element, this);
    }
    else if (dynamic_cast<Note *>(element)) {
        dc->StartGraphic(element, "", element->GetUuid());
        DrawNote(dc, element, layer, staff, measure);
        dc->EndGraphic(element, this);
    }
    else if (dynamic_cast<Rest *>(element)) {
        dc->StartGraphic(element, "", element->GetUuid());
        DrawRest(dc, element, layer, staff, measure);
        dc->EndGraphic(element, this);
    }
}

void View::DrawFlag(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    Flag *flag = dynamic_cast<Flag *>(element);
    assert(flag);

    Stem *stem = dynamic_cast<Stem *>(flag->GetFirstParent(STEM));
    assert(stem);

    int x = flag->GetDrawingX() - m_doc->GetDrawingStemWidth(staff->m_drawingStaffSize) / 2;
    int y = flag->GetDrawingY();

    dc->StartGraphic(element, "", element->GetUuid());

    wchar_t code = flag->GetSmuflCode(stem->GetDrawingStemDir());
    DrawSmuflCode(dc, x, y, code, staff->m_drawingStaffSize, flag->IsCueSize());

    dc->EndGraphic(element, this);
}

void View::DrawKeySig(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    KeySig *keySig = dynamic_cast<KeySig *>(element);
    assert(keySig);

    int symb;
    int x, y, i;

    Clef *c = layer->GetClef(element);
    if (!c) {
        keySig->SetEmptyBB();
        return;
    }

    // hidden key signature
    if (!keySig->m_drawingShow) {
        keySig->SetEmptyBB();
        return;
    }

    // C major (0) key sig and no cancellation
    else if ((keySig->GetAlterationNumber() == 0) && (keySig->m_drawingCancelAccidCount == 0)) {
        keySig->SetEmptyBB();
        return;
    }

    // C major (0) key sig and system scoreDef - cancellation (if any) is done at the end of the previous system
    else if ((keySig->GetScoreDefRole() == SYSTEM_SCOREDEF) && (keySig->GetAlterationNumber() == 0)) {
        keySig->SetEmptyBB();
        return;
    }

    dc->StartGraphic(element, "", element->GetUuid());

    x = element->GetDrawingX();
    // HARDCODED
    int step = m_doc->GetGlyphWidth(SMUFL_E262_accidentalSharp, staff->m_drawingStaffSize, false) * TEMP_KEYSIG_STEP;

    int clefLocOffset = layer->GetClefLocOffset(element);
    int loc;

    // Show cancellation if C major (0) or if any cancellation and show cancellation (showchange) is true (false by
    // default)
    if ((keySig->GetScoreDefRole() != SYSTEM_SCOREDEF)
        && ((keySig->GetAlterationNumber() == 0) || keySig->m_drawingShowchange)) {
        // The type of alteration is different (f/s or f/n or s/n) - cancel all accid in the normal order
        if (keySig->GetAlterationType() != keySig->m_drawingCancelAccidType) {
            for (i = 0; i < keySig->m_drawingCancelAccidCount; i++) {
                data_PITCHNAME pitch = KeySig::GetAlterationAt(keySig->m_drawingCancelAccidType, i);
                loc = PitchInterface::CalcLoc(
                    pitch, KeySig::GetOctave(keySig->m_drawingCancelAccidType, pitch, c), clefLocOffset);
                y = staff->GetDrawingY() + staff->CalcPitchPosYRel(m_doc, loc);

                DrawSmuflCode(dc, x, y, SMUFL_E261_accidentalNatural, staff->m_drawingStaffSize, false);
                x += step;
            }
        }
        // Cancel some of them if more accid before
        else if (keySig->GetAlterationNumber() < keySig->m_drawingCancelAccidCount) {
            for (i = keySig->GetAlterationNumber(); i < keySig->m_drawingCancelAccidCount; i++) {
                data_PITCHNAME pitch = KeySig::GetAlterationAt(keySig->m_drawingCancelAccidType, i);
                loc = PitchInterface::CalcLoc(
                    pitch, KeySig::GetOctave(keySig->m_drawingCancelAccidType, pitch, c), clefLocOffset);
                y = staff->GetDrawingY() + staff->CalcPitchPosYRel(m_doc, loc);

                DrawSmuflCode(dc, x, y, SMUFL_E261_accidentalNatural, staff->m_drawingStaffSize, false);
                x += step;
            }
        }
    }

    for (i = 0; i < keySig->GetAlterationNumber(); i++) {
        data_PITCHNAME pitch = KeySig::GetAlterationAt(keySig->GetAlterationType(), i);
        loc = PitchInterface::CalcLoc(pitch, KeySig::GetOctave(keySig->GetAlterationType(), pitch, c), clefLocOffset);
        y = staff->GetDrawingY() + staff->CalcPitchPosYRel(m_doc, loc);

        if (keySig->GetAlterationType() == ACCIDENTAL_EXPLICIT_f)
            symb = SMUFL_E260_accidentalFlat;
        else
            symb = SMUFL_E262_accidentalSharp;

        DrawSmuflCode(dc, x, y, symb, staff->m_drawingStaffSize, false);
        x += step;
    }

    dc->EndGraphic(element, this);
}

void View::DrawMeterSig(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    MeterSig *meterSig = dynamic_cast<MeterSig *>(element);
    assert(meterSig);

    dc->StartGraphic(element, "", element->GetUuid());

    int y = staff->GetDrawingY() - m_doc->GetDrawingUnit(staff->m_drawingStaffSize) * (staff->m_drawingLines - 1);
    int x = element->GetDrawingX();

    if (meterSig->GetForm() == meterSigVis_FORM_invis) {
        // just skip
    }
    else if (meterSig->HasSym()) {
        if (meterSig->GetSym() == METERSIGN_common) {
            DrawSmuflCode(dc, element->GetDrawingX(), y, SMUFL_E08A_timeSigCommon, staff->m_drawingStaffSize, false);
        }
        else if (meterSig->GetSym() == METERSIGN_cut) {
            DrawSmuflCode(dc, element->GetDrawingX(), y, SMUFL_E08B_timeSigCutCommon, staff->m_drawingStaffSize, false);
        }
        x += m_doc->GetDrawingUnit(staff->m_drawingStaffSize) * 5; // step forward because we have a symbol
    }
    else if (meterSig->GetForm() == meterSigVis_FORM_num) {
        DrawMeterSigFigures(dc, x, staff->GetDrawingY(), meterSig->GetCount(), NONE, staff);
    }
    else if (meterSig->HasCount()) {
        DrawMeterSigFigures(dc, x, staff->GetDrawingY(), meterSig->GetCount(), meterSig->GetUnit(), staff);
    }

    dc->EndGraphic(element, this);
}

void View::DrawMRest(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    MRest *mRest = dynamic_cast<MRest *>(element);
    assert(mRest);

    dc->StartGraphic(element, "", element->GetUuid());

    mRest->CenterDrawingX();

    int y = element->GetDrawingY();

    if (measure->m_measureAligner.GetMaxTime() >= (DUR_MAX * 2)) {
        y -= m_doc->GetDrawingDoubleUnit(staff->m_drawingStaffSize);
        DrawRestBreve(dc, mRest->GetDrawingX(), y, staff);
    }
    else
        DrawRestWhole(dc, mRest->GetDrawingX(), y, DUR_1, false, staff);

    if (mRest->HasFermata()) {
        DrawFermataAttr(dc, element, layer, staff);
    }

    dc->EndGraphic(element, this);
}

void View::DrawMRpt(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    MRpt *mRpt = dynamic_cast<MRpt *>(element);
    assert(mRpt);

    mRpt->CenterDrawingX();

    dc->StartGraphic(element, "", element->GetUuid());

    DrawMRptPart(dc, element->GetDrawingX(), SMUFL_E500_repeat1Bar, mRpt->m_drawingMeasureCount, false, staff);

    dc->EndGraphic(element, this);
}

void View::DrawMRpt2(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    MRpt2 *mRpt2 = dynamic_cast<MRpt2 *>(element);
    assert(mRpt2);

    mRpt2->CenterDrawingX();

    dc->StartGraphic(element, "", element->GetUuid());

    DrawMRptPart(dc, element->GetDrawingX(), SMUFL_E501_repeat2Bars, 2, true, staff);

    dc->EndGraphic(element, this);
}

void View::DrawMultiRest(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    MultiRest *multiRest = dynamic_cast<MultiRest *>(element);
    assert(multiRest);

    multiRest->CenterDrawingX();

    int x1, x2, y1, y2, length;

    dc->StartGraphic(element, "", element->GetUuid());

    int width = measure->GetInnerWidth();
    int xCentered = multiRest->GetDrawingX();

    // We do not support more than three chars
    int num = std::min(multiRest->GetNum(), 999);

    if (num > 2 || multiRest->GetBlock() == true) {
        // This is 1/2 the length of the black rectangle
        length = width - 2 * m_doc->GetDrawingDoubleUnit(staff->m_drawingStaffSize);

        // a is the central point, claculate x and x2
        x1 = xCentered - length / 2;
        x2 = xCentered + length / 2;

        // Position centered in staff
        y2 = staff->GetDrawingY() - m_doc->GetDrawingUnit(staff->m_drawingStaffSize) * staff->m_drawingLines;
        y1 = y2 + m_doc->GetDrawingDoubleUnit(staff->m_drawingStaffSize);

        // Draw the base rect
        DrawFilledRectangle(dc, x1, y1, x2, y2);

        // Draw two lines at beginning and end
        int border = m_doc->GetDrawingUnit(staff->m_drawingStaffSize);
        DrawFilledRectangle(
            dc, x1, y1 + border, x1 + m_doc->GetDrawingStemWidth(staff->m_drawingStaffSize) * 2, y2 - border);
        DrawFilledRectangle(
            dc, x2 - m_doc->GetDrawingStemWidth(staff->m_drawingStaffSize) * 2, y1 + border, x2, y2 - border);
    }
    else {
        // Draw the base rect
        x1 = xCentered - m_doc->GetDrawingDoubleUnit(staff->m_drawingStaffSize) / 3;
        x2 = xCentered + m_doc->GetDrawingDoubleUnit(staff->m_drawingStaffSize) / 3;

        // Position centered in staff
        y1 = staff->GetDrawingY()
            - m_doc->GetDrawingDoubleUnit(staff->m_drawingStaffSize) * int((staff->m_drawingLines) / 2);
        if (staff->m_drawingLines > 1) y1 += m_doc->GetDrawingDoubleUnit(staff->m_drawingStaffSize);
        y2 = y1 - m_doc->GetDrawingDoubleUnit(staff->m_drawingStaffSize);
        if (num == 2)
            DrawFilledRectangle(dc, x1, y1 + 4, x2, y2 - 4);
        else
            DrawRestWhole(dc, xCentered, y1, DUR_1, false, staff);
    }

    // Draw the text above
    int start_offset = 0; // offset from x to center text

    // convert to string
    std::wstring wtext = IntToTimeSigFigures(num);

    dc->SetFont(m_doc->GetDrawingSmuflFont(staff->m_drawingStaffSize, false));
    TextExtend extend;
    dc->GetSmuflTextExtent(wtext, &extend);
    start_offset = (x2 - x1 - extend.m_width) / 2; // calculate offset to center text
    int y = (staff->GetDrawingY() > y1) ? staff->GetDrawingY() + 3 * m_doc->GetDrawingUnit(staff->m_drawingStaffSize)
                                        : y1 + 3 * m_doc->GetDrawingUnit(staff->m_drawingStaffSize);
    DrawSmuflString(dc, x1 + start_offset, y, wtext, false);
    dc->ResetFont();

    dc->EndGraphic(element, this);
}

void View::DrawMultiRpt(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    MultiRpt *multiRpt = dynamic_cast<MultiRpt *>(element);
    assert(multiRpt);

    multiRpt->CenterDrawingX();

    dc->StartGraphic(element, "", element->GetUuid());

    DrawMRptPart(dc, element->GetDrawingX(), SMUFL_E501_repeat2Bars, multiRpt->GetNum(), true, staff);

    dc->EndGraphic(element, this);

    return;
}

void View::DrawNote(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    Note *note = dynamic_cast<Note *>(element);
    assert(note);

    if (note->IsMensural()) {
        DrawMensuralNote(dc, element, layer, staff, measure);
        return;
    }

    if (note->m_crossStaff) staff = note->m_crossStaff;

    bool drawingCueSize = note->IsCueSize();
    int staffSize = staff->m_drawingStaffSize;
    int noteY = element->GetDrawingY();
    int noteX = element->GetDrawingX();
    int noteXShift;
    int drawingDur;
    wchar_t fontNo;

    drawingDur = note->GetDrawingDur();
    drawingDur = ((note->GetColored() == BOOLEAN_true) && drawingDur > DUR_1) ? (drawingDur + 1) : drawingDur;

    int radius = note->GetDrawingRadius(m_doc, staffSize, drawingCueSize);

    noteXShift = -radius;

    /************** Noteheads: **************/

    if (drawingDur < DUR_1) {
        DrawMaximaToBrevis(dc, noteY, element, layer, staff);
    }
    // Whole notes
    else if (drawingDur == DUR_1) {
        if (note->GetColored() == BOOLEAN_true)
            fontNo = SMUFL_E0FA_noteheadWholeFilled;
        else
            fontNo = SMUFL_E0A2_noteheadWhole;

        DrawSmuflCode(dc, noteX + noteXShift, noteY, fontNo, staff->m_drawingStaffSize, drawingCueSize, true);
    }
    // Other values
    else {
        if ((note->GetColored() == BOOLEAN_true) || drawingDur == DUR_2)
            fontNo = SMUFL_E0A3_noteheadHalf;
        else
            fontNo = SMUFL_E0A4_noteheadBlack;

        DrawSmuflCode(dc, noteX + noteXShift, noteY, fontNo, staff->m_drawingStaffSize, drawingCueSize, true);
    }

    /************ Draw children (accidentals, etc) ************/

    DrawLayerChildren(dc, note, layer, staff, measure);

    /************** peripherals: **************/

    if (note->GetDrawingTieAttr()) {
        System *system = dynamic_cast<System *>(measure->GetFirstParent(SYSTEM));
        // create a placeholder for the tie attribute that will be drawn from the system
        dc->StartGraphic(note->GetDrawingTieAttr(), "", note->GetDrawingTieAttr()->GetUuid().c_str());
        dc->EndGraphic(note->GetDrawingTieAttr(), this);
        if (system) system->AddToDrawingList(note->GetDrawingTieAttr());
    }

    if (note->HasFermata()) {
        DrawFermataAttr(dc, element, layer, staff);
    }
}

void View::DrawRest(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    Rest *rest = dynamic_cast<Rest *>(element);
    assert(rest);

    if (rest->IsMensural()) {
        DrawMensuralRest(dc, element, layer, staff, measure);
        return;
    }

    if (rest->m_crossStaff) staff = rest->m_crossStaff;

    bool drawingCueSize = rest->IsCueSize();
    int drawingDur = rest->GetActualDur();

    int x = element->GetDrawingX();
    int y = element->GetDrawingY();

    switch (drawingDur) {
        case DUR_LG: DrawRestLong(dc, x, y, staff); break;
        case DUR_BR: DrawRestBreve(dc, x, y, staff); break;
        case DUR_1:
        case DUR_2: DrawRestWhole(dc, x, y, drawingDur, drawingCueSize, staff); break;
        default:
            x -= m_doc->GetGlyphWidth(rest->GetRestGlyph(), staff->m_drawingStaffSize, drawingCueSize) / 2;
            y += m_doc->GetDrawingDoubleUnit(staff->m_drawingStaffSize);
            DrawSmuflCode(dc, x, y, rest->GetRestGlyph(), staff->m_drawingStaffSize, drawingCueSize);
    }

    /************ Draw children (dots) ************/

    DrawLayerChildren(dc, rest, layer, staff, measure);

    /************** peripherals: **************/

    if (rest->HasFermata()) {
        DrawFermataAttr(dc, element, layer, staff);
    }
}

void View::DrawSpace(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    dc->StartGraphic(element, "", element->GetUuid());
    dc->DrawPlaceholder(ToDeviceContextX(element->GetDrawingX()), ToDeviceContextY(element->GetDrawingY()));
    dc->EndGraphic(element, this);
}

void View::DrawStem(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    Stem *stem = dynamic_cast<Stem *>(element);
    assert(stem);

    dc->StartGraphic(element, "", element->GetUuid());

    DrawFilledRectangle(dc, stem->GetDrawingX() - m_doc->GetDrawingStemWidth(staff->m_drawingStaffSize) / 2,
        stem->GetDrawingY(), stem->GetDrawingX() + m_doc->GetDrawingStemWidth(staff->m_drawingStaffSize) / 2,
        stem->GetDrawingY() - stem->GetDrawingStemLen());

    DrawLayerChildren(dc, stem, layer, staff, measure);
    
    /************ Draw slash ************/
    
    if (stem->GetGrace() == GRACE_unacc) {
        DrawAcciaccaturaSlash(dc, stem, staff);
    }

    dc->EndGraphic(element, this);
}

void View::DrawSyl(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    Syl *syl = dynamic_cast<Syl *>(element);
    assert(syl);

    if (!syl->GetStart()) {
        LogWarning("Parent note for <syl> was not found");
        return;
    }

    // move the position back - to be updated HARDCODED also see View::DrawSylConnector and View::DrawSylConnectorLines
    // assert(false);
    syl->SetDrawingXRel(-m_doc->GetDrawingUnit(staff->m_drawingStaffSize) * 2);
    syl->SetDrawingYRel(GetSylYRel(syl, staff));

    dc->StartGraphic(syl, "", syl->GetUuid());
    dc->DeactivateGraphicY();

    dc->SetBrush(m_currentColour, AxSOLID);

    FontInfo currentFont;
    if (staff->m_drawingStaffDef) {
        currentFont = *m_doc->GetDrawingLyricFont(staff->m_drawingStaffSize);
        dc->SetFont(&currentFont);
    }
    else {
        dc->SetFont(m_doc->GetDrawingLyricFont(staff->m_drawingStaffSize));
    }

    bool setX = false;
    bool setY = false;
    int x = syl->GetDrawingX();
    int y = syl->GetDrawingY();

    dc->StartText(ToDeviceContextX(x), ToDeviceContextY(y));
    DrawTextChildren(dc, syl, ToDeviceContextX(x), ToDeviceContextY(y), setX, setY);
    dc->EndText();

    dc->ResetFont();
    dc->ResetBrush();

    if (syl->GetStart() && syl->GetEnd()) {
        System *currentSystem = dynamic_cast<System *>(measure->GetFirstParent(SYSTEM));
        // Postpone the drawing of the syl to the end of the system; this will call DrawSylConnector
        // that will look if the last note is in the same system (or not) and draw the connectors accordingly
        if (currentSystem) {
            currentSystem->AddToDrawingList(syl);
        }
    }

    dc->ReactivateGraphic();
    dc->EndGraphic(syl, this);
}

void View::DrawTuplet(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    Tuplet *tuplet = dynamic_cast<Tuplet *>(element);
    assert(tuplet);

    dc->StartGraphic(element, "", element->GetUuid());

    // Draw the inner elements
    DrawLayerChildren(dc, tuplet, layer, staff, measure);

    // Add to the list of postponed element
    layer->AddToDrawingList(tuplet);

    dc->EndGraphic(element, this);
}

void View::DrawVerse(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff, Measure *measure)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);
    assert(measure);

    Verse *verse = dynamic_cast<Verse *>(element);
    assert(verse);

    dc->StartGraphic(verse, "", verse->GetUuid());

    DrawLayerChildren(dc, verse, layer, staff, measure);

    dc->EndGraphic(verse, this);
}

//----------------------------------------------------------------------------
// Partial drawing methods
//----------------------------------------------------------------------------

void View::DrawAcciaccaturaSlash(DeviceContext *dc, Stem *stem, Staff *staff)
{
    assert(dc);
    assert(stem);
    assert(staff);

    dc->SetPen(AxBLACK, m_doc->GetDrawingStemWidth(staff->m_drawingStaffSize), AxSOLID);
    dc->SetBrush(AxBLACK, AxSOLID);

    int positionShift = m_doc->GetCueSize(m_doc->GetDrawingUnit(staff->m_drawingStaffSize));
    int positionShiftX1 = positionShift * 3 / 2;
    int positionShiftY1 = positionShift * -5;
    int positionShiftX2 = positionShift * 3;
    int positionShiftY2 = positionShift * -1;
    Point startPoint(stem->GetDrawingX(), stem->GetDrawingY() - stem->GetDrawingStemLen());

    int startPointY = startPoint.y;

    // HARDCODED
    if (stem->GetDrawingStemDir() == STEMDIRECTION_up) {
        dc->DrawLine(ToDeviceContextX(startPoint.x - positionShiftX1), ToDeviceContextY(startPointY + positionShiftY1),
            ToDeviceContextX(startPoint.x + positionShiftX2), ToDeviceContextY(startPointY + positionShiftY2));
    }
    else {
        dc->DrawLine(ToDeviceContextX(startPoint.x - positionShiftX1), ToDeviceContextY(startPointY - positionShiftY2),
            ToDeviceContextX(startPoint.x + positionShiftX2), ToDeviceContextY(startPointY - positionShiftY1));
    }

    dc->ResetPen();
    dc->ResetBrush();
}

void View::DrawDotsPart(DeviceContext *dc, int x, int y, unsigned char dots, Staff *staff)
{
    int i;

    if (IsOnStaffLine(y, staff)) {
        y += m_doc->GetDrawingUnit(staff->m_drawingStaffSize);
    }
    for (i = 0; i < dots; i++) {
        DrawDot(dc, x, y, staff->m_drawingStaffSize);
        // HARDCODED
        x += m_doc->GetDrawingUnit(staff->m_drawingStaffSize) * 1.5;
    }
}

void View::DrawFermataAttr(DeviceContext *dc, LayerElement *element, Layer *layer, Staff *staff)
{
    assert(dc);
    assert(element);
    assert(layer);
    assert(staff);

    int x, y;

    x = element->GetDrawingX();

    // We move the fermata position of half of the fermata size
    x -= m_doc->GetGlyphWidth(SMUFL_E4C0_fermataAbove, staff->m_drawingStaffSize, false) / 2;

    AttFermatapresent *fermatapresent = dynamic_cast<AttFermatapresent *>(element);
    assert(fermatapresent);
    data_PLACE place = fermatapresent->GetFermata();

    // First case, notes
    if (element->Is({ NOTE, CHORD })) {
        if (place == PLACE_above) {
            // check if the notehead is in the staff.
            int top = element->GetDrawingTop(m_doc, staff->m_drawingStaffSize, true, ARTIC_PART_OUTSIDE);
            if (top < staff->GetDrawingY()) {
                // in the staff, set the fermata 20 pixels above the last line
                y = staff->GetDrawingY() + m_doc->GetDrawingUnit(staff->m_drawingStaffSize);
            }
            else {
                // out of the staff, place the trill above the notehead
                y = top + m_doc->GetDrawingUnit(staff->m_drawingStaffSize);
            }
            // draw the up-fermata - need cue size support
            DrawSmuflCode(dc, x, y, SMUFL_E4C0_fermataAbove, staff->m_drawingStaffSize, false);
        }
        else {
            int bottom = element->GetDrawingBottom(m_doc, staff->m_drawingStaffSize, true, ARTIC_PART_OUTSIDE);
            // This works as above, only we check that the note head is not
            if (bottom > (staff->GetDrawingY() - m_doc->GetDrawingStaffSize(staff->m_drawingStaffSize))) {
                // notehead in staff, set  under
                y = staff->GetDrawingY() - m_doc->GetDrawingStaffSize(staff->m_drawingStaffSize)
                    - m_doc->GetDrawingUnit(staff->m_drawingStaffSize);
            }
            else {
                // notehead under staff, set under notehead
                y = bottom - m_doc->GetDrawingUnit(staff->m_drawingStaffSize);
            }
            // draw the down-fermata - need cue size support
            DrawSmuflCode(dc, x, y, SMUFL_E4C1_fermataBelow, staff->m_drawingStaffSize, false);
        }
    }
    else if (element->Is({ REST, MREST })) {
        if (place == PLACE_above) {
            y = staff->GetDrawingY() + m_doc->GetDrawingDoubleUnit(staff->m_drawingStaffSize);
            DrawSmuflCode(dc, x, y, SMUFL_E4C0_fermataAbove, staff->m_drawingStaffSize, false);
        }
        else {
            y = staff->GetDrawingY() - m_doc->GetDrawingStaffSize(staff->m_drawingStaffSize)
                - m_doc->GetDrawingUnit(staff->m_drawingStaffSize);
            DrawSmuflCode(dc, x, y, SMUFL_E4C1_fermataBelow, staff->m_drawingStaffSize, false);
        }
    }
}

void View::DrawMeterSigFigures(DeviceContext *dc, int x, int y, int num, int numBase, Staff *staff)
{
    assert(dc);
    assert(staff);

    int ynum = 0, yden = 0;
    std::wstring numText, numBaseText;

    int yCenter = y - (staff->m_drawingLines) / 2 * m_doc->GetDrawingDoubleUnit(staff->m_drawingStaffSize);
    yCenter += m_doc->GetDrawingDoubleUnit(staff->m_drawingStaffSize);

    if (numBase) {
        ynum = yCenter; // - (m_doc->GetDrawingUnit(staff->m_drawingStaffSize) * 2);
        yden = ynum - (m_doc->GetDrawingDoubleUnit(staff->m_drawingStaffSize) * 2);
    }
    else
        ynum = yCenter - (m_doc->GetDrawingUnit(staff->m_drawingStaffSize) * 2);

    dc->SetFont(m_doc->GetDrawingSmuflFont(staff->m_drawingStaffSize, false));

    numText = IntToTimeSigFigures(num);
    if (numBase) numBaseText = IntToTimeSigFigures(numBase);

    std::wstring widthText = (numText.length() > numBaseText.length()) ? numText : numBaseText;

    TextExtend extend;
    dc->GetSmuflTextExtent(widthText, &extend);
    x += (extend.m_width / 2);
    DrawSmuflString(dc, x, ynum, numText, true, staff->m_drawingStaffSize);

    if (numBase) {
        DrawSmuflString(dc, x, yden, numBaseText, true, staff->m_drawingStaffSize);
    }

    dc->ResetFont();
}

void View::DrawMRptPart(DeviceContext *dc, int xCentered, wchar_t smuflCode, int num, bool line, Staff *staff)
{
    int xSymbol = xCentered - m_doc->GetGlyphWidth(smuflCode, staff->m_drawingStaffSize, false) / 2;
    int y = staff->GetDrawingY();
    int ySymbol = y - staff->m_drawingLines / 2 * m_doc->GetDrawingDoubleUnit(staff->m_drawingStaffSize);

    DrawSmuflCode(dc, xSymbol, ySymbol, smuflCode, staff->m_drawingStaffSize, false);

    if (line) {
        DrawVerticalLine(dc, y, y - m_doc->GetDrawingStaffSize(staff->m_drawingStaffSize), xCentered,
            m_doc->GetDrawingBarLineWidth(staff->m_drawingStaffSize));
    }

    if (num > 0) {
        dc->SetFont(m_doc->GetDrawingSmuflFont(staff->m_drawingStaffSize, false));
        // calculate the width of the figures
        TextExtend extend;
        std::wstring figures = IntToTupletFigures(num);
        dc->GetSmuflTextExtent(figures, &extend);
        int y = (staff->GetDrawingY() > ySymbol)
            ? staff->GetDrawingY() + m_doc->GetDrawingUnit(staff->m_drawingStaffSize)
            : ySymbol + 3 * m_doc->GetDrawingUnit(staff->m_drawingStaffSize);
        dc->DrawMusicText(figures, ToDeviceContextX(xCentered - extend.m_width / 2), ToDeviceContextY(y));
        dc->ResetFont();
    }
}

void View::DrawRestBreve(DeviceContext *dc, int x, int y, Staff *staff)
{
    int x1, x2, y1, y2;
    y1 = y;
    x1 = x;
    x2 = x + m_doc->GetDrawingUnit(staff->m_drawingStaffSize);

    y2 = y1 + m_doc->GetDrawingDoubleUnit(staff->m_drawingStaffSize);
    DrawFilledRectangle(dc, x1, y2, x2, y1);

    // lines
    x1 = x - m_doc->GetDrawingUnit(staff->m_drawingStaffSize);
    x2 = x + m_doc->GetDrawingUnit(staff->m_drawingStaffSize);

    DrawHorizontalLine(dc, x1, x2, y2, 1);
    DrawHorizontalLine(dc, x1, x2, y1, 1);
}

void View::DrawRestLong(DeviceContext *dc, int x, int y, Staff *staff)
{
    int x1, x2, y1, y2;

    x1 = x;
    x2 = x + m_doc->GetDrawingUnit(staff->m_drawingStaffSize);

    y1 = y - m_doc->GetDrawingDoubleUnit(staff->m_drawingStaffSize);
    y2 = y + m_doc->GetDrawingDoubleUnit(staff->m_drawingStaffSize);

    DrawFilledRectangle(dc, x1, y2, x2, y1);
}

void View::DrawRestWhole(DeviceContext *dc, int x, int y, int valeur, bool cueSize, Staff *staff)
{
    int x1, x2, y1, y2, vertic;
    y1 = y;
    vertic = m_doc->GetDrawingUnit(staff->m_drawingStaffSize);

    int off
        = m_doc->GetDrawingLedgerLineLength(staff->m_drawingStaffSize, cueSize) * 2 / 3; // i.e., la moitie de la ronde

    x1 = x - off;
    x2 = x + off;

    if (valeur == DUR_1) vertic = -vertic;

    // look if on line or between line
    if ((y - staff->GetDrawingY()) % m_doc->GetDrawingDoubleUnit(staff->m_drawingStaffSize)) {
        if (valeur == DUR_2)
            y1 -= vertic;
        else
            y1 += vertic;
    }

    // legder line
    if (y > (int)staff->GetDrawingY()
        || y < staff->GetDrawingY()
                - (staff->m_drawingLines - 1) * m_doc->GetDrawingDoubleUnit(staff->m_drawingStaffSize))
        DrawHorizontalLine(
            dc, x1 - off / 2, x2 + off / 2, y1, m_doc->GetDrawingStaffLineWidth(staff->m_drawingStaffSize) * 1.75);

    y2 = y1 + vertic;
    DrawFilledRectangle(dc, x1, y1, x2, y2);
}

//----------------------------------------------------------------------------
// Calculation or preparation methods
///----------------------------------------------------------------------------

int View::GetSylYRel(Syl *syl, Staff *staff)
{
    assert(syl && staff);

    int y = 0;
    StaffAlignment *aligment = staff->GetAlignment();
    if (aligment) {
        FontInfo *lyricFont = m_doc->GetDrawingLyricFont(staff->m_drawingStaffSize);
        int descender = -m_doc->GetTextGlyphDescender(L'q', lyricFont, false);
        int height = m_doc->GetTextGlyphHeight(L'I', lyricFont, false);
        int margin = m_doc->GetBottomMargin(SYL) * m_doc->GetDrawingUnit(staff->m_drawingStaffSize) / PARAM_DENOMINATOR;

        y = -aligment->GetStaffHeight() - aligment->GetOverflowBelow()
            + (aligment->GetVerseCount() - syl->m_drawingVerse) * (height + descender + margin) + (descender);
    }
    return y;
}

bool View::IsOnStaffLine(int y, Staff *staff)
{
    // int y1 = y - staff->GetDrawingY();
    // int y2 = m_doc->GetDrawingUnit(staff->m_drawingStaffSize);
    // LogDebug("IsOnStaff %d %d", y1, y2);

    return ((y - staff->GetDrawingY()) % (2 * m_doc->GetDrawingUnit(staff->m_drawingStaffSize)) == 0);
}

int View::GetNearestInterStaffPosition(int y, Staff *staff, data_STAFFREL place)
{
    int yPos = y - staff->GetDrawingY();
    int distance = yPos % m_doc->GetDrawingUnit(staff->m_drawingStaffSize);
    if (place == STAFFREL_above) {
        if (distance > 0) distance = m_doc->GetDrawingUnit(staff->m_drawingStaffSize) - distance;
        return y - distance + m_doc->GetDrawingUnit(staff->m_drawingStaffSize);
    }
    else {
        if (distance < 0) distance = m_doc->GetDrawingUnit(staff->m_drawingStaffSize) + distance;
        return y - distance - m_doc->GetDrawingUnit(staff->m_drawingStaffSize);
    }
}

} // namespace vrv
