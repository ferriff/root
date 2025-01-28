// @(#)root/graf:$Id$
// Author: Georg Troska 19/05/16

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include <cstdlib>

#include "TBuffer.h"
#include "TCandle.h"
#include "TVirtualPad.h"
#include "TH2D.h"
#include "TRandom2.h"
#include "TStyle.h"
#include "strlcpy.h"

ClassImp(TCandle);

/** \class TCandle
\ingroup BasicGraphics

The candle plot painter class.

Instances of this class are generated by the histograms painting
classes (THistPainter and THStack) when an candle plot (box plot) is drawn.
TCandle is the "painter class" of the box plots. Therefore it is never used
directly to draw a candle.
*/

////////////////////////////////////////////////////////////////////////////////
/// TCandle default constructor.

TCandle::TCandle()
{
   fIsCalculated  = false;
   fIsRaw         = false;
   fPosCandleAxis = 0.;
   fCandleWidth   = 1.0;
   fHistoWidth    = 1.0;
   fMean          = 0.;
   fMedian        = 0.;
   fMedianErr     = 0;
   fBoxUp         = 0.;
   fBoxDown       = 0.;
   fWhiskerUp     = 0.;
   fWhiskerDown   = 0.;
   fNDatapoints   = 0;
   fDismiss       = false;
   fLogX          = 0;
   fLogY          = 0;
   fLogZ          = 0;
   fNDrawPoints   = 0;
   fNHistoPoints  = 0;
   fAxisMin       = 0.;
   fAxisMax       = 0.;
   fOption        = kNoOption;
   fProj          = nullptr;
   fDatapoints    = nullptr;

}

////////////////////////////////////////////////////////////////////////////////
/// TCandle constructor passing a draw-option.

TCandle::TCandle(const char *opt)
{
   fIsCalculated  = false;
   fIsRaw         = false;
   fPosCandleAxis = 0.;
   fCandleWidth   = 1.0;
   fHistoWidth    = 1.0;
   fMean          = 0.;
   fMedian        = 0.;
   fMedianErr     = 0;
   fBoxUp         = 0.;
   fBoxDown       = 0.;
   fWhiskerUp     = 0.;
   fWhiskerDown   = 0.;
   fNDatapoints   = 0;
   fDismiss       = false;
   fLogX          = 0;
   fLogY          = 0;
   fLogZ          = 0;
   fNDrawPoints   = 0;
   fNHistoPoints  = 0;
   fAxisMin       = 0.;
   fAxisMax       = 0.;
   fOption        = kNoOption;
   fProj          = nullptr;
   fDatapoints    = nullptr;

   // Conversion necessary in order to cast from const char* to char*
   char myopt[128];
   strlcpy(myopt,opt,128);


   ParseOption(myopt);
}


////////////////////////////////////////////////////////////////////////////////
/// TCandle constructor for raw-data candles.

TCandle::TCandle(const Double_t candlePos, const Double_t candleWidth, Long64_t n, Double_t * points)
   : TAttLine(), TAttFill(), TAttMarker()
{
   //Preliminary values only, need to be calculated before paint
   fMean          = 0;
   fMedian        = 0;
   fMedianErr     = 0;
   fBoxUp         = 0;
   fBoxDown       = 0;
   fWhiskerUp     = 0;
   fWhiskerDown   = 0;
   fNDatapoints   = n;
   fIsCalculated  = false;
   fIsRaw         = true;
   fPosCandleAxis = candlePos;
   fCandleWidth   = candleWidth;
   fHistoWidth    = candleWidth;
   fDatapoints    = points;
   fProj          = nullptr;
   fDismiss       = false;
   fOption        = kNoOption;
   fLogX          = 0;
   fLogY          = 0;
   fLogZ          = 0;
   fNDrawPoints   = 0;
   fNHistoPoints  = 0;
   fAxisMin       = 0.;
   fAxisMax       = 0.;
}

////////////////////////////////////////////////////////////////////////////////
/// TCandle TH1 data constructor.

TCandle::TCandle(const Double_t candlePos, const Double_t candleWidth, TH1D *proj)
   : TAttLine(), TAttFill(), TAttMarker()
{
   //Preliminary values only, need to be calculated before paint
   fMean          = 0;
   fMedian        = 0;
   fMedianErr     = 0;
   fBoxUp         = 0;
   fBoxDown       = 0;
   fWhiskerUp     = 0;
   fWhiskerDown   = 0;
   fNDatapoints   = 0;
   fIsCalculated  = false;
   fIsRaw         = false;
   fPosCandleAxis = candlePos;
   fCandleWidth   = candleWidth;
   fHistoWidth    = candleWidth;
   fDatapoints    = nullptr;
   fProj          = proj;
   fDismiss       = false;
   fOption        = kNoOption;
   fLogX          = 0;
   fLogY          = 0;
   fLogZ          = 0;
   fNDrawPoints   = 0;
   fNHistoPoints  = 0;
   fAxisMin       = 0.;
   fAxisMax       = 0.;
}

////////////////////////////////////////////////////////////////////////////////
/// TCandle default destructor.

TCandle::~TCandle()
{
   if (fIsRaw && fProj) delete fProj;
}

////////////////////////////////////////////////////////////////////////////////
/// Returns true if candle plot should be scaled

Bool_t TCandle::IsCandleScaled() const
{
   return gStyle->GetCandleScaled();
}

////////////////////////////////////////////////////////////////////////////////
/// Returns true if violin plot should be scaled

Bool_t TCandle::IsViolinScaled() const
{
   return gStyle->GetViolinScaled();
}

////////////////////////////////////////////////////////////////////////////////
/// Static function to set WhiskerRange, by setting whisker-range, one can force
/// the whiskers to cover the fraction of the distribution.
/// Set wRange between 0 and 1. Default is 1
/// TCandle::SetWhiskerRange(0.95) will set all candle-charts to cover 95% of
/// the distribution with the whiskers.
/// Can only be used with the standard-whisker definition

void TCandle::SetWhiskerRange(const Double_t wRange)
{
   gStyle->SetCandleWhiskerRange(wRange);
}

////////////////////////////////////////////////////////////////////////////////
/// Static function to set BoxRange, by setting box-range, one can force the
/// box of the candle-chart to cover that given fraction of the distribution.
/// Set bRange between 0 and 1. Default is 0.5
/// TCandle::SetBoxRange(0.68) will set all candle-charts to cover 68% of the
/// distribution by the box

void TCandle::SetBoxRange(const Double_t bRange)
{
   gStyle->SetCandleBoxRange(bRange);
}

////////////////////////////////////////////////////////////////////////////////
/// Static function to set scaling between candles-withs. A candle containing
/// 100 entries will be two times wider than a candle containing 50 entries

void TCandle::SetScaledCandle(const Bool_t cScale)
{
   gStyle->SetCandleScaled(cScale);
}

////////////////////////////////////////////////////////////////////////////////
/// Static function to set scaling between violin-withs. A violin or histo chart
/// with a maximum bin content to 100 will be two times as high as a violin with
/// a maximum bin content of 50

void TCandle::SetScaledViolin(const Bool_t vScale)
{
   gStyle->SetViolinScaled(vScale);
}

////////////////////////////////////////////////////////////////////////////////
/// Parsing of the option-string.
/// The option-string will be empty at the end (by-reference).

int TCandle::ParseOption(char * opt) {
   fOption = kNoOption;
   char *l = strstr(opt,"CANDLE");

   if (l) {
      const CandleOption fallbackCandle = (CandleOption)(kBox + kMedianLine + kMeanCircle + kWhiskerAll + kAnchor);

      char direction = ' ';
      char preset = ' ';

      if (l[6] >= 'A' && l[6] <= 'Z') direction = l[6];
      if (l[6] >= '1' && l[6] <= '9') preset = l[6];
      if (l[7] >= 'A' && l[7] <= 'Z' && preset != ' ') direction = l[7];
      if (l[7] >= '1' && l[7] <= '9' && direction != ' ') preset = l[7];

      if (direction == 'X' || direction == 'V') { /* nothing */ }
      if (direction == 'Y' || direction == 'H') { fOption = (CandleOption)(fOption + kHorizontal); }
      if (preset == '1') //Standard candle using old candle-definition
         fOption = (CandleOption)(fOption + fallbackCandle);
      else if (preset == '2') //New standard candle with better whisker definition + outlier
         fOption = (CandleOption)(fOption + kBox + kMeanLine + kMedianLine + kWhisker15 + kAnchor + kPointsOutliers);
      else if (preset == '3')  //Like candle2 but with a fMean as a circle
         fOption = (CandleOption)(fOption + kBox + kMeanCircle + kMedianLine + kWhisker15 + kAnchor + kPointsOutliers);
      else if (preset == '4')  //Like candle3 but showing the uncertainty of the fMedian as well
         fOption = (CandleOption)(fOption + kBox + kMeanCircle + kMedianNotched + kWhisker15 + kAnchor + kPointsOutliers);
      else if (preset == '5')  //Like candle2 but showing all datapoints
         fOption = (CandleOption)(fOption + kBox + kMeanLine + kMedianLine + kWhisker15 + kAnchor + kPointsAll);
      else if (preset == '6')  //Like candle2 but showing all datapoints scattered
         fOption = (CandleOption)(fOption + kBox + kMeanCircle + kMedianLine + kWhisker15 + kAnchor + kPointsAllScat);
      else if (preset != ' ') //For all other presets not implemented yet used fallback candle
         fOption = (CandleOption)(fOption + fallbackCandle);

      if (preset != ' ' && direction != ' ')
         memcpy(l,"        ",8);
      else if (preset != ' ' || direction != ' ')
         memcpy(l,"       ",7);
      else
         memcpy(l,"      ",6);

      Bool_t useIndivOption = false;

      if (direction == ' ') direction = 'X';
      if (preset == ' ') { // Check if the user wants to set the properties individually
         char *brOpen = strstr(opt,"(");
         char *brClose = strstr(opt,")");
         char indivOption[32];
         if (brOpen && brClose) {
            useIndivOption = true;
            bool wasHorizontal = IsHorizontal();
            strlcpy(indivOption, brOpen, brClose-brOpen+2); //Now the string "(....)" including brackets is in this array
            sscanf(indivOption,"(%d)", (int*) &fOption);
            if (wasHorizontal && !IsHorizontal()) {fOption = (CandleOption)(fOption + kHorizontal);}
            memcpy(brOpen,"                ",brClose-brOpen+1); //Cleanup

            fOptionStr.Form("CANDLE%c(%ld)", direction, (long)fOption);
         } else {
            fOption = (CandleOption)(fOption + fallbackCandle);
         }
      } else {
         fOptionStr.Form("CANDLE%c%c",direction,preset);
      }
      //Handle option "CANDLE" ,"CANDLEX" or "CANDLEY" to behave like "CANDLEX1" or "CANDLEY1"
      if (!useIndivOption && !fOption ) {
         fOption = fallbackCandle;
         fOptionStr.Form("CANDLE%c2",direction);
      }
   }

   l = strstr(opt,"VIOLIN");
   if (l) {
      const CandleOption fallbackCandle = (CandleOption)(kMeanCircle + kWhiskerAll + kHistoViolin + kHistoZeroIndicator);

      char direction = ' ';
      char preset = ' ';

      if (l[6] >= 'A' && l[6] <= 'Z') direction = l[6];
      if (l[6] >= '1' && l[6] <= '9') preset = l[6];
      if (l[7] >= 'A' && l[7] <= 'Z' && preset != ' ') direction = l[7];
      if (l[7] >= '1' && l[7] <= '9' && direction != ' ') preset = l[7];

      if (direction == 'X' || direction == 'V') { /* nothing */ }
      if (direction == 'Y' || direction == 'H') { fOption = (CandleOption)(fOption + kHorizontal); }
      if (preset == '1') //Standard candle using old candle-definition
         fOption = (CandleOption)(fOption + fallbackCandle);
      else if (preset == '2') //New standard candle with better whisker definition + outlier
         fOption = (CandleOption)(fOption + kMeanCircle + kWhisker15 + kHistoViolin + kHistoZeroIndicator + kPointsOutliers);
      else if (preset != ' ') //For all other presets not implemented yet used fallback candle
         fOption = (CandleOption)(fOption + fallbackCandle);

      if (preset != ' ' && direction != ' ')
         memcpy(l,"        ",8);
      else if (preset != ' ' || direction != ' ')
         memcpy(l,"       ",7);
      else
         memcpy(l,"      ",6);

      Bool_t useIndivOption = false;

      if (direction == ' ') direction = 'X';
      if (preset == ' ') { // Check if the user wants to set the properties individually
         char *brOpen = strstr(opt,"(");
         char *brClose = strstr(opt,")");
         char indivOption[32];
         if (brOpen && brClose) {
            useIndivOption = true;
            bool wasHorizontal = IsHorizontal();
            strlcpy(indivOption, brOpen, brClose-brOpen +2); //Now the string "(....)" including brackets is in this array
            sscanf(indivOption,"(%d)", (int*) &fOption);
            if (wasHorizontal && !IsHorizontal()) {fOption = (CandleOption)(fOption + kHorizontal);}
            memcpy(brOpen,"                ",brClose-brOpen+1); //Cleanup

            fOptionStr.Form("VIOLIN%c(%ld)", direction, (long)fOption);
         } else {
            fOption = (CandleOption)(fOption + fallbackCandle);
         }
      } else {
         fOptionStr.Form("VIOLIN%c%c", direction, preset);
      }
      //Handle option "VIOLIN" ,"VIOLINX" or "VIOLINY" to behave like "VIOLINX1" or "VIOLINY1"
      if (!useIndivOption && !fOption ) {
         fOption = fallbackCandle;
         fOptionStr.Form("VIOLIN%c1", direction);
      }
   }

   fIsCalculated = false;

   return fOption;

}

////////////////////////////////////////////////////////////////////////////////
/// Calculates all values needed by the candle definition depending on the
/// candle options.

void TCandle::Calculate() {
   //Reset everything
   fNDrawPoints = 0;
   fNHistoPoints = 0;

   Bool_t swapXY = IsOption(kHorizontal);
   Bool_t doLogY = (!(swapXY) && fLogY) || (swapXY && fLogX);
   Bool_t doLogX = (!(swapXY) && fLogX) || (swapXY && fLogY);
   Bool_t doLogZ = fLogZ;

   //Will be min and max values of raw-data
   Double_t min = 1e15;
   Double_t max = -1e15;

   // Determining the quantiles
   Double_t prob[5];

   Double_t wRange = gStyle->GetCandleWhiskerRange();
   Double_t bRange = gStyle->GetCandleBoxRange();


   if (wRange >= 1) {
      prob[0] = 1e-15;
      prob[4] = 1-1e-15;
   } else {
      prob[0] = 0.5 - wRange/2.;
      prob[4] = 0.5 + wRange/2.;
   }

   if (bRange >= 1) {
      prob[1] = 1E-14;
      prob[3] = 1-1E-14;
   } else {
      prob[1] = 0.5 - bRange/2.;
      prob[3] = 0.5 + bRange/2.;
   }

   prob[2] = 0.5;

   Double_t quantiles[5] = { 0., 0., 0., 0., 0. };
   if (!fIsRaw && fProj) { //Need a calculation for a projected histo
      if (((IsOption(kHistoLeft)) || (IsOption(kHistoRight)) || (IsOption(kHistoViolin))) && fProj->GetNbinsX() > 500) {
         // When using the histooption the number of bins of the projection is
         // limited because of the array space defined by kNMAXPOINTS.
         // So the histo is rebinned, that it can be displayed at any time.
         // Finer granularity is not useful anyhow
         int divideBy = ((fProj->GetNbinsX() - 1)/((kNMAXPOINTS-10)/4))+1;
         fProj->RebinX(divideBy);
      }
      fProj->GetQuantiles(5, quantiles, prob);
   } else { //Need a calculation for a raw-data candle
      TMath::Quantiles(fNDatapoints,5,fDatapoints,quantiles,prob,kFALSE);
   }

   // Check if the quantiles are valid, seems the under- and overflow is taken
   // into account as well, we need to ignore this!
   if (quantiles[0] >= quantiles[4] ||
       quantiles[1] >= quantiles[3]) {
      return;
   }

   // Definition of the candle in the standard case
   fBoxUp = quantiles[3];
   fBoxDown = quantiles[1];
   fWhiskerUp = quantiles[4]; //Standard case
   fWhiskerDown = quantiles[0]; //Standard case
   fMedian = quantiles[2];
   Double_t iqr = fBoxUp-fBoxDown;
   Int_t nOutliers = 0;

   if (IsOption(kWhisker15)) { // Improved whisker definition, with 1.5*iqr
      if (!fIsRaw && fProj) { //Need a calculation for a projected histo
         int bin = fProj->FindBin(fBoxDown-1.5*iqr);
         // extending only to the lowest data value within this range
         while (fProj->GetBinContent(bin) == 0 && bin <= fProj->GetNbinsX()) bin++;
         fWhiskerDown = fProj->GetXaxis()->GetBinLowEdge(bin);

         bin = fProj->FindBin(fBoxUp+1.5*iqr);
         while (fProj->GetBinContent(bin) == 0 && bin >= 1) bin--;
         fWhiskerUp = fProj->GetXaxis()->GetBinUpEdge(bin);
      } else { //Need a calculation for a raw-data candle
         fWhiskerUp = fBoxDown;
         fWhiskerDown = fBoxUp;

         //Need to find highest value up to 1.5*iqr from the BoxUp-pos, and the lowest value up to -1.5*iqr from the boxLow-pos
         for (Long64_t i = 0; i < fNDatapoints; ++i) {
            Double_t myData = fDatapoints[i];
            if (myData > fWhiskerUp && myData <= fBoxUp + 1.5*iqr) fWhiskerUp = myData;
            if (myData < fWhiskerDown && myData >= fBoxDown - 1.5*iqr) fWhiskerDown = myData;
         }
      }
   }

   if (!fIsRaw && fProj) { //Need a calculation for a projected histo
      fMean = fProj->GetMean();
      fMedianErr = 1.57*iqr/sqrt(fProj->GetEntries());
      fAxisMin = fProj->GetXaxis()->GetXmin();
      fAxisMax = fProj->GetXaxis()->GetXmax();
   } else { //Need a calculation for a raw-data candle
      //Calculate the Mean
      fMean = 0;
      for (Long64_t i = 0; i < fNDatapoints; ++i) {
         fMean += fDatapoints[i];
         if (fDatapoints[i] < min) min = fDatapoints[i];
         if (fDatapoints[i] > max) max = fDatapoints[i];
         if (fDatapoints[i] < fWhiskerDown || fDatapoints[i] > fWhiskerUp) nOutliers++;
      }
      fMean /= fNDatapoints;
      fMedianErr = 1.57*iqr/sqrt(fNDatapoints);
   }

   //Doing the outliers and other single points to show
   if (GetCandleOption(5) > 0) { //Draw outliers
      TRandom2 random;
      const int maxOutliers = kNMAXPOINTS;
      Double_t myScale = 1.;
      if (!fIsRaw && fProj) { //Need a calculation for a projected histo
         if (fProj->GetEntries() > maxOutliers/2) myScale = fProj->GetEntries()/(maxOutliers/2.);
         fNDrawPoints = 0;
         for (int bin = 0; bin < fProj->GetNbinsX(); bin++) {
            // Either show them only outside the whiskers, or all of them
            if (fProj->GetBinContent(bin) > 0 && (fProj->GetBinCenter(bin) < fWhiskerDown || fProj->GetBinCenter(bin) > fWhiskerUp || (GetCandleOption(5) > 1)) ) {
               Double_t scaledBinContent = fProj->GetBinContent(bin)/myScale;
               if (scaledBinContent >0 && scaledBinContent < 1) scaledBinContent = 1; //Outliers have a typical bin content between 0 and 1, when scaling they would disappear
               for (int j=0; j < (int)scaledBinContent; j++) {
                  if (fNDrawPoints > maxOutliers) break;
                  if (IsOption(kPointsAllScat)) { //Draw outliers and "all" values scattered
                     fDrawPointsX[fNDrawPoints] = fPosCandleAxis - fCandleWidth/2. + fCandleWidth*random.Rndm();
                     fDrawPointsY[fNDrawPoints] = fProj->GetBinLowEdge(bin) + fProj->GetBinWidth(bin)*random.Rndm();
                  } else { //Draw them in the "candle line"
                     fDrawPointsX[fNDrawPoints] = fPosCandleAxis;
                     if ((int)scaledBinContent == 1) //If there is only one datapoint available put it in the middle of the bin
                        fDrawPointsY[fNDrawPoints] = fProj->GetBinCenter(bin);
                     else //If there is more than one datapoint scatter it along the bin, otherwise all marker would be (invisibly) stacked on top of each other
                        fDrawPointsY[fNDrawPoints] = fProj->GetBinLowEdge(bin) + fProj->GetBinWidth(bin)*random.Rndm();
                  }
                  if (swapXY) {
                     //Swap X and Y
                     Double_t keepCurrently;
                     keepCurrently = fDrawPointsX[fNDrawPoints];
                     fDrawPointsX[fNDrawPoints] = fDrawPointsY[fNDrawPoints];
                     fDrawPointsY[fNDrawPoints] = keepCurrently;
                  }
                     // Continue fMeans, that fNDrawPoints is not increased, so that value will not be shown
                  if (doLogX) {
                     if (fDrawPointsX[fNDrawPoints] > 0) fDrawPointsX[fNDrawPoints] = TMath::Log10(fDrawPointsX[fNDrawPoints]); else continue;
                  }
                  if (doLogY) {
                     if (fDrawPointsY[fNDrawPoints] > 0) fDrawPointsY[fNDrawPoints] = TMath::Log10(fDrawPointsY[fNDrawPoints]); else continue;
                  }
                  fNDrawPoints++;
               }
            }
            if (fNDrawPoints > maxOutliers) { //Should never happen, due to myScale!!!
               Error ("PaintCandlePlot","Not possible to draw all outliers.");
               break;
            }
         }
      } else { //Raw data candle
         //If only outliers are shown, calculate myScale only based on nOutliers, use fNDatapoints (all) instead
         if (IsOption(kPointsOutliers) && nOutliers > maxOutliers/2) {
            myScale = nOutliers/(maxOutliers/2.);
         } else {
            if (fNDatapoints > maxOutliers/2) myScale = fNDatapoints/(maxOutliers/2.);
         }
         fNDrawPoints = 0;
         for (int i = 0; i < fNDatapoints; i++ ) {
            Double_t myData = fDatapoints[i];
            Double_t maxScatter = (fWhiskerUp-fWhiskerDown)/100;
            if (!(i % (int) myScale == 0 )) continue; //If the amount of data is too large take only every 2nd or 3rd to reduce the amount
            // Either show them only outside the whiskers, or all of them
            if (myData < fWhiskerDown || myData > fWhiskerUp || (GetCandleOption(5) > 1)) {
               if (IsOption(kPointsAllScat)) { //Draw outliers and "all" values scattered
                  fDrawPointsX[fNDrawPoints] = fPosCandleAxis - fCandleWidth/2. + fCandleWidth*random.Rndm();
                  fDrawPointsY[fNDrawPoints] = myData + (random.Rndm() - 0.5)*maxScatter; //random +- 0.5 of candle-height
               } else { //Draw them in the "candle line"
                  fDrawPointsX[fNDrawPoints] = fPosCandleAxis;
                  fDrawPointsY[fNDrawPoints] = myData + (random.Rndm() - 0.5)*maxScatter; //random +- 0.5 of candle-height
               }
               if (swapXY) {
                  //Swap X and Y
                  Double_t keepCurrently;
                  keepCurrently = fDrawPointsX[fNDrawPoints];
                  fDrawPointsX[fNDrawPoints] = fDrawPointsY[fNDrawPoints];
                  fDrawPointsY[fNDrawPoints] = keepCurrently;
               }
               // Continue fMeans, that fNDrawPoints is not increased, so that value will not be shown
               if (doLogX) {
                  if (fDrawPointsX[fNDrawPoints] > 0) fDrawPointsX[fNDrawPoints] = TMath::Log10(fDrawPointsX[fNDrawPoints]);
                  else continue;
               }
               if (doLogY) {
                  if (fDrawPointsY[fNDrawPoints] > 0) fDrawPointsY[fNDrawPoints] = TMath::Log10(fDrawPointsY[fNDrawPoints]);
                  else continue;
               }
               fNDrawPoints++;
               if (fNDrawPoints > maxOutliers) { //Should never happen, due to myScale!!!
                  Error ("PaintCandlePlotRaw","Not possible to draw all outliers.");
                  break;
               }
            }
         }
      }
   }
   if (IsOption(kHistoRight) || IsOption(kHistoLeft) || IsOption(kHistoViolin)) {
      //We are starting with kHistoRight, left will be modified from right later
      if (fIsRaw) { //This is a raw-data candle
         if (!fProj) {
            fProj = new TH1D("hpa","hpa",100,min,max+0.0001*(max-min));
            fProj->SetDirectory(nullptr);
            for (Long64_t i = 0; i < fNDatapoints; ++i) {
               fProj->Fill(fDatapoints[i]);
            }
         }
      }

      fNHistoPoints = 0;
      Double_t maxContent = fProj->GetMaximum();
      Double_t maxHistoHeight = fHistoWidth;
      if (IsOption(kHistoViolin)) maxHistoHeight *= 0.5;

      bool isFirst = true;
      int lastNonZero = 0;
      for (int bin = 1; bin <= fProj->GetNbinsX(); bin++) {
         if (isFirst) {
            if (fProj->GetBinContent(bin) > 0) {
               fHistoPointsX[fNHistoPoints] = fPosCandleAxis;
               fHistoPointsY[fNHistoPoints] = fProj->GetBinLowEdge(bin);
                if (doLogX) {
                  if (fHistoPointsX[fNHistoPoints] > 0) fHistoPointsX[fNHistoPoints] = TMath::Log10(fHistoPointsX[fNHistoPoints]); else continue;
               }
               if (doLogY) {
                  if (fHistoPointsY[fNHistoPoints] > 0) fHistoPointsY[fNHistoPoints] = TMath::Log10(fHistoPointsY[fNHistoPoints]); else continue;
               }
               fNHistoPoints++;
               isFirst = false;
            } else {
               continue;
            }
         }

         Double_t myBinValue = fProj->GetBinContent(bin);
         if (doLogZ) {
            if (myBinValue > 0) myBinValue = TMath::Log10(myBinValue); else myBinValue = 0;
         }
         fHistoPointsX[fNHistoPoints] = fPosCandleAxis + myBinValue/maxContent*maxHistoHeight;
         fHistoPointsY[fNHistoPoints] = fProj->GetBinLowEdge(bin);
         fNHistoPoints++;
         fHistoPointsX[fNHistoPoints] = fPosCandleAxis + myBinValue/maxContent*maxHistoHeight;
         fHistoPointsY[fNHistoPoints] = fProj->GetBinLowEdge(bin)+fProj->GetBinWidth(bin);
         if (doLogX) {
            if (fHistoPointsX[fNHistoPoints -1] > 0) fHistoPointsX[fNHistoPoints - 1] = TMath::Log10(fHistoPointsX[fNHistoPoints - 1]); else continue;
            if (fHistoPointsX[fNHistoPoints] > 0) fHistoPointsX[fNHistoPoints] = TMath::Log10(fHistoPointsX[fNHistoPoints]); else continue;
         }
         if (doLogY) {
            if (fHistoPointsY[fNHistoPoints -1] > 0) fHistoPointsY[fNHistoPoints - 1] = TMath::Log10(fHistoPointsY[fNHistoPoints - 1]); else continue;
            if (fHistoPointsY[fNHistoPoints] > 0) fHistoPointsY[fNHistoPoints] = TMath::Log10(fHistoPointsY[fNHistoPoints]); else continue;
         }

         fNHistoPoints++;
         if (fProj->GetBinContent(bin) > 0) lastNonZero = fNHistoPoints;
      }

      fHistoPointsX[fNHistoPoints] = fPosCandleAxis;
      fHistoPointsY[fNHistoPoints] = fHistoPointsY[fNHistoPoints-1];
      fNHistoPoints = lastNonZero+1; //+1 so that the line down to 0 is added as well

      if (IsOption(kHistoLeft)) {
         for (int i = 0; i < fNHistoPoints; i++) {
            fHistoPointsX[i] = 2*fPosCandleAxis - fHistoPointsX[i];
         }
      }
      if (IsOption(kHistoViolin)) {
         for (int i = 0; i < fNHistoPoints; i++) {
            fHistoPointsX[fNHistoPoints + i] = 2*fPosCandleAxis - fHistoPointsX[fNHistoPoints -i-1];
            fHistoPointsY[fNHistoPoints + i] = fHistoPointsY[fNHistoPoints -i-1];
         }
         fNHistoPoints *= 2;
      }
   }

   fIsCalculated = true;
}

////////////////////////////////////////////////////////////////////////////////
/// Paint one candle with its current attributes.

void TCandle::Paint(Option_t *)
{
   if (!gPad) return;

   //If something was changed before, we need to recalculate some values
   if (!fIsCalculated) Calculate();

   // Save the attributes as they were set originally
   Style_t saveLine   = GetLineStyle();
   Style_t saveMarker = GetMarkerStyle();
   Style_t saveFillStyle = GetFillStyle();
   Style_t saveFillColor = GetFillColor();
   Style_t saveLineColor = GetLineColor();

   Double_t dimLeft = fPosCandleAxis-0.5*fCandleWidth;
   Double_t dimRight = fPosCandleAxis+0.5*fCandleWidth;

   TAttLine::Modify();
   TAttFill::Modify();
   TAttMarker::Modify();

   Bool_t swapXY = IsOption(kHorizontal);
   Bool_t doLogY = (!(swapXY) && fLogY) || (swapXY && fLogX);
   Bool_t doLogX = (!(swapXY) && fLogX) || (swapXY && fLogY);

   // From now on this is real painting only, no calculations anymore

   if (IsOption(kHistoZeroIndicator)) {
      SetLineColor(saveFillColor);
      TAttLine::Modify();
      PaintLine(fPosCandleAxis, fAxisMin, fPosCandleAxis, fAxisMax, swapXY);
      SetLineColor(saveLineColor);
      TAttLine::Modify();
   }


   if (IsOption(kHistoRight) || IsOption(kHistoLeft) || IsOption(kHistoViolin)) {
      if (IsOption(kHistoZeroIndicator) && (saveFillStyle != 0)) {
         SetLineColor(saveFillColor);
         TAttLine::Modify();
      }
      if (!swapXY) {
         gPad->PaintFillArea(fNHistoPoints, fHistoPointsX, fHistoPointsY);
         gPad->PaintPolyLine(fNHistoPoints, fHistoPointsX, fHistoPointsY);
      } else {
         gPad->PaintFillArea(fNHistoPoints, fHistoPointsY, fHistoPointsX);
         gPad->PaintPolyLine(fNHistoPoints, fHistoPointsY, fHistoPointsX);
      }
      if (IsOption(kHistoZeroIndicator) && (saveFillStyle != 0)) {
         SetLineColor(saveLineColor);
         TAttLine::Modify();
      }
   }

   if (IsOption(kBox)) { // Draw a simple box
     if (IsOption(kMedianNotched)) { // Check if we have to draw a box with notches
         Double_t x[] = {dimLeft,  dimLeft, dimLeft+fCandleWidth/3., dimLeft, dimLeft, dimRight,
                         dimRight, dimRight-fCandleWidth/3., dimRight, dimRight, dimLeft};
         Double_t y[] = {fBoxDown, fMedian-fMedianErr, fMedian, fMedian+fMedianErr, fBoxUp, fBoxUp,
                         fMedian+fMedianErr, fMedian, fMedian-fMedianErr, fBoxDown, fBoxDown};
         PaintBox(11, x, y, swapXY);
      } else { // draw a simple box
         Double_t x[] = {dimLeft, dimLeft, dimRight, dimRight, dimLeft};
         Double_t y[] = {fBoxDown,  fBoxUp, fBoxUp,  fBoxDown,   fBoxDown};
         PaintBox(5, x, y, swapXY);
      }
   }

   if (IsOption(kAnchor)) { // Draw the anchor line
      PaintLine(dimLeft, fWhiskerUp, dimRight, fWhiskerUp, swapXY);
      PaintLine(dimLeft, fWhiskerDown, dimRight, fWhiskerDown, swapXY);
   }

   if (IsOption(kWhiskerAll) && !IsOption(kHistoZeroIndicator)) { // Whiskers are dashed
      SetLineStyle(2);
      TAttLine::Modify();
      PaintLine(fPosCandleAxis, fWhiskerUp, fPosCandleAxis, fBoxUp, swapXY);
      PaintLine(fPosCandleAxis, fBoxDown, fPosCandleAxis, fWhiskerDown, swapXY);
      SetLineStyle(saveLine);
      TAttLine::Modify();
   }  else if ((IsOption(kWhiskerAll) && IsOption(kHistoZeroIndicator)) || IsOption(kWhisker15) ) { // Whiskers without dashing, better whisker definition, or forced when using zero line
      PaintLine(fPosCandleAxis, fWhiskerUp, fPosCandleAxis, fBoxUp, swapXY);
      PaintLine(fPosCandleAxis, fBoxDown, fPosCandleAxis, fWhiskerDown, swapXY);
   }

   if (IsOption(kMedianLine)) { // Paint fMedian as a line
      PaintLine(dimLeft, fMedian, dimRight, fMedian, swapXY);
   } else if (IsOption(kMedianNotched)) { // Paint fMedian as a line (using notches, fMedian line is shorter)
      PaintLine(dimLeft+fCandleWidth/3, fMedian, dimRight-fCandleWidth/3., fMedian, swapXY);
   } else if (IsOption(kMedianCircle)) { // Paint fMedian circle
      Double_t myMedianX[1], myMedianY[1];
      if (!swapXY) {
         myMedianX[0] = fPosCandleAxis;
         myMedianY[0] = fMedian;
      } else {
         myMedianX[0] = fMedian;
         myMedianY[0] = fPosCandleAxis;
      }

      Bool_t isValid = true;
      if (doLogX) {
         if (myMedianX[0] > 0) myMedianX[0] = TMath::Log10(myMedianX[0]); else isValid = false;
      }
      if (doLogY) {
         if (myMedianY[0] > 0) myMedianY[0] = TMath::Log10(myMedianY[0]); else isValid = false;
      }
      Int_t mw = gStyle->GetCandleCircleLineWidth();
      if (mw == 1) SetMarkerStyle(24);
      else         SetMarkerStyle(18*mw+17);
      TAttMarker::Modify();

      if (isValid) gPad->PaintPolyMarker(1,myMedianX,myMedianY); // A circle for the fMedian

      SetMarkerStyle(saveMarker);
      TAttMarker::Modify();

   }

   if (IsOption(kMeanCircle)) { // Paint fMean as a circle
      Double_t myMeanX[1], myMeanY[1];
      if (!swapXY) {
         myMeanX[0] = fPosCandleAxis;
         myMeanY[0] = fMean;
      } else {
         myMeanX[0] = fMean;
         myMeanY[0] = fPosCandleAxis;
      }

      Bool_t isValid = true;
      if (doLogX) {
         if (myMeanX[0] > 0) myMeanX[0] = TMath::Log10(myMeanX[0]); else isValid = false;
      }
      if (doLogY) {
         if (myMeanY[0] > 0) myMeanY[0] = TMath::Log10(myMeanY[0]); else isValid = false;
      }

      Int_t mw = gStyle->GetCandleCircleLineWidth();
      if (mw == 1) SetMarkerStyle(24);
      else         SetMarkerStyle(18*mw+17);
      TAttMarker::Modify();

      if (isValid) gPad->PaintPolyMarker(1,myMeanX,myMeanY); // A circle for the fMean

      SetMarkerStyle(saveMarker);
      TAttMarker::Modify();

   } else if (IsOption(kMeanLine)) { // Paint fMean as a dashed line
      SetLineStyle(2);
      TAttLine::Modify();

      PaintLine(dimLeft, fMean, dimRight, fMean, swapXY);
      SetLineStyle(saveLine);
      TAttLine::Modify();

   }

   if (IsOption(kAnchor)) { //Draw standard anchor
      PaintLine(dimLeft, fWhiskerDown, dimRight, fWhiskerDown, swapXY); // the lower anchor line
      PaintLine(dimLeft, fWhiskerUp, dimRight, fWhiskerUp, swapXY); // the upper anchor line
   }

   // This is a bit complex. All values here are handled as outliers. Usually
   // only the datapoints outside the whiskers are shown.
   // One can show them in one row as crosses, or scattered randomly. If activated
   // all datapoint are shown in the same way

   if (GetCandleOption(5) > 0) { //Draw outliers
      if (IsOption(kPointsAllScat)) { //Draw outliers and "all" values scattered
         SetMarkerStyle(0);
      } else {
         Int_t mw = gStyle->GetCandleCrossLineWidth();
         if (mw == 1) SetMarkerStyle(5);
         else         SetMarkerStyle(18*mw+16);
      }
      TAttMarker::Modify();
      gPad->PaintPolyMarker(fNDrawPoints,fDrawPointsX, fDrawPointsY);
   }
}

////////////////////////////////////////////////////////////////////////////////
/// Return true is this option is activated in fOption

bool TCandle::IsOption(CandleOption opt) const {
   long myOpt = 9;
   int pos = 0;
   for (pos = 0; pos < 16; pos++) {
      if (myOpt > opt) break;
      else myOpt *=10;
   }
   myOpt /= 9;
   int thisOpt = GetCandleOption(pos);

   return (thisOpt * myOpt) == opt;
}

////////////////////////////////////////////////////////////////////////////////
/// Paint a box for candle.

void TCandle::PaintBox(Int_t nPoints, Double_t *x, Double_t *y, Bool_t swapXY)
{
   if (!gPad) return;

   Bool_t doLogY = (!(swapXY) && fLogY) || (swapXY && fLogX);
   Bool_t doLogX = (!(swapXY) && fLogX) || (swapXY && fLogY);
   if (doLogY) {
      for (int i=0; i<nPoints; i++) {
         if (y[i] > 0) y[i] = TMath::Log10(y[i]);
         else return;
      }
   }
   if (doLogX) {
      for (int i=0; i<nPoints; i++) {
         if (x[i] > 0) x[i] = TMath::Log10(x[i]);
         else return;
      }
   }
   if (!swapXY) {
      gPad->PaintFillArea(nPoints, x, y);
      gPad->PaintPolyLine(nPoints, x, y);
   } else {
      gPad->PaintFillArea(nPoints, y, x);
      gPad->PaintPolyLine(nPoints, y, x);
   }
}

////////////////////////////////////////////////////////////////////////////////
/// Paint a line for candle.

void TCandle::PaintLine(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Bool_t swapXY)
{
   if (!gPad) return;

   Bool_t doLogY = (!(swapXY) && fLogY) || (swapXY && fLogX);
   Bool_t doLogX = (!(swapXY) && fLogX) || (swapXY && fLogY);
   if (doLogY) {
      if (y1 > 0) y1 = TMath::Log10(y1); else return;
      if (y2 > 0) y2 = TMath::Log10(y2); else return;
   }
   if (doLogX) {
      if (x1 > 0) x1 = TMath::Log10(x1); else return;
      if (x2 > 0) x2 = TMath::Log10(x2); else return;
   }
   if (!swapXY) {
      gPad->PaintLine(x1, y1, x2, y2);
   } else {
      gPad->PaintLine(y1, x1, y2, x2);
   }
}

////////////////////////////////////////////////////////////////////////////////
/// Stream an object of class TCandle.

void TCandle::Streamer(TBuffer &R__b)
{
   if (R__b.IsReading()) {
      UInt_t R__s, R__c;
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
      if (R__v > 3) {
         R__b.ReadClassBuffer(TCandle::Class(), this, R__v, R__s, R__c);
         return;
      }
   } else {
      R__b.WriteClassBuffer(TCandle::Class(),this);
   }
}

////////////////////////////////////////////////////////////////////////////////
/// The coordinates in the TParallelCoordVar-class are in Pad-Coordinates, so we need to convert them

void TCandle::ConvertToPadCoords(Double_t minAxis, Double_t maxAxis, Double_t axisMinCoord, Double_t axisMaxCoord)
{
   if (!fIsCalculated) Calculate();
   Double_t a,b;
   if (fLogY) {
      a = TMath::Log10(minAxis);
      b = TMath::Log10(maxAxis/minAxis);
   } else {
      a = minAxis;
      b = maxAxis-minAxis;
   }

   fMean        = axisMinCoord + ((fMean-a)/b)*(axisMaxCoord-axisMinCoord);
   fMedian      = axisMinCoord + ((fMedian-a)/b)*(axisMaxCoord-axisMinCoord);
   fMedianErr   = axisMinCoord + ((fMedianErr-a)/b)*(axisMaxCoord-axisMinCoord);
   fBoxUp       = axisMinCoord + ((fBoxUp-a)/b)*(axisMaxCoord-axisMinCoord);
   fBoxDown     = axisMinCoord + ((fBoxDown-a)/b)*(axisMaxCoord-axisMinCoord);
   fWhiskerUp   = axisMinCoord + ((fWhiskerUp-a)/b)*(axisMaxCoord-axisMinCoord);
   fWhiskerDown = axisMinCoord + ((fWhiskerDown-a)/b)*(axisMaxCoord-axisMinCoord);

   for (int i = 0; i < fNDrawPoints; i++) {
      fDrawPointsY[i] = axisMinCoord + ((fDrawPointsY[i]-a)/b)*(axisMaxCoord-axisMinCoord);
  }
   for (int i = 0; i < fNHistoPoints; i++) {
      fHistoPointsY[i] = axisMinCoord + ((fHistoPointsY[i]-a)/b)*(axisMaxCoord-axisMinCoord);
   }
}
