// -*- C++ -*-
#ifndef ALIANALYSISTASKDG_H
#define ALIANALYSISTASKDG_H

class TH1;
class TTree;
class TList;

class AliESDHeader;
class AliESDAD;
class AliESDVZERO;

class AliESDtrack;
class AliESDtrackCuts;

#include <TObject.h>
#include <TString.h>
#include <TBits.h>
#include <TClonesArray.h>

#include "AliESDEvent.h"
#include "AliESDVertex.h"
#include "AliAnalysisTaskSE.h"
#include "AliTOFHeader.h"
#include "AliTriggerAnalysis.h"
#include "AliPID.h"
#include "AliPIDResponse.h"
#include "AliAnalysisUtils.h"

class AliAnalysisTaskDG : public AliAnalysisTaskSE {
public:

  AliAnalysisTaskDG(const char *name="AliAnalysisTaskDG");
  virtual ~AliAnalysisTaskDG();
  
  virtual void UserCreateOutputObjects();
  virtual void UserExec(Option_t* option);
  virtual void Terminate(Option_t *);
  virtual void NotifyRun();

  void SetBranchNames(TString options) { fTreeBranchNames = options; }
  void SetTrackCutType(TString tc) { fTrackCutType = tc; }
  void SetTriggerSelection(TString ts) { fTriggerSelection = ts; }
  void SetCDBStorage(TString s) { fCDBStorage = s; }

  TString GetListName() const { return fTrackCutType+"_TL"; }
  TString GetTreeName() const { return fTrackCutType+"_TE"; }
  TString GetResultsFileName() const { return "results.root"; }

  struct EventInfo {
    EventInfo()
      : fClassMask(0)
      , fClassMaskNext50(0)
      , fBCID(0)
      , fPeriod(0)
      , fTimeStamp(0)
      , fL0Inputs(0)
      , fL1Inputs(0)
      , fnTrk(0)
      , fnTrklet(0)
      , fCharge(0)
      , fL2Inputs(0)
      , fOrbitID(0) {}

    void Fill(const AliESDHeader *);

    ULong64_t fClassMask;
    ULong64_t fClassMaskNext50;
    UInt_t    fBCID;
    UInt_t    fPeriod;
    UInt_t    fTimeStamp;
    UInt_t    fL0Inputs;
    UInt_t    fL1Inputs;
    UInt_t    fnTrk;
    UInt_t    fnTrklet;
    Int_t     fCharge;
    UShort_t  fL2Inputs;
    UShort_t  fOrbitID;  

  } ;

  struct ADV0 {
    enum {
      kCside = 0,
      kAside = 1
    };

    ADV0() {
      for (Int_t i=0; i<2; ++i) {
	fTime[i] = -10240.0f;
	fBB[i] = fBG[i] = -1;
	fDecisionOnline[i] = fDecisionOffline[i] = -1;
      }
    }

    void FillAD(const AliESDEvent *, AliTriggerAnalysis &);
    void FillV0(const AliESDEvent *, AliTriggerAnalysis &);

    void FillInvalid();

    Float_t  fTime[2];            //
    Char_t   fBB[2];              // 
    Char_t   fBG[2];              //
    Char_t   fDecisionOnline[2];  //
    Char_t   fDecisionOffline[2]; //

  } ;

  class TreeData : public TObject {
  public:
    TreeData() 
      : TObject()
      , fEventInfo()
      , fV0Info()
      , fADInfo()
      , fIsIncompleteDAQ(kFALSE)
      , fIsSPDClusterVsTrackletBG(kFALSE)
      , fIskMB(kFALSE) {}

    EventInfo fEventInfo;
    ADV0      fV0Info;
    ADV0      fADInfo;
    Bool_t    fIsIncompleteDAQ;
    Bool_t    fIsSPDClusterVsTrackletBG;
    Bool_t    fIskMB;
    ClassDef(TreeData, 1);
  } ;

  struct TrackData : public TObject {
    TrackData(AliESDtrack *tr=NULL, AliPIDResponse *pidResponse=NULL)
      : TObject()
      , fSign(0)
      , fPx(0)
      , fPy(0)
      , fPz(0)
      , fITSsignal(0)
      , fTPCsignal(0)
      , fTOFsignal(0) {
      fPIDStatus[0] = fPIDStatus[1] = fPIDStatus[2] = AliPIDResponse::kDetNoSignal;
      for (Int_t i=0; i<AliPID::kSPECIES; ++i) {
	fNumSigmaITS[i] = fNumSigmaTPC[i] = fNumSigmaTOF[i] = -999.0f;
      }
      Fill(tr, pidResponse);
    }

    void Fill(AliESDtrack *, AliPIDResponse*);

    Int_t   fSign;
    Float_t fPx,fPy,fPz;
    Float_t fITSsignal, fTPCsignal, fTOFsignal;
    Float_t fNumSigmaITS[AliPID::kSPECIES];
    Float_t fNumSigmaTPC[AliPID::kSPECIES];
    Float_t fNumSigmaTOF[AliPID::kSPECIES];
    Char_t  fPIDStatus[3]; // ITS,TPC,TOF
    ClassDef(TrackData, 1);
  } ;


protected:
  void SetBranches(TTree* t);

  enum {
    kHistTrig,
    kNHist
  };

private:  
  AliAnalysisTaskDG(const AliAnalysisTaskDG&); // not implemented
  AliAnalysisTaskDG& operator=(const AliAnalysisTaskDG&); // not implemented

  TString          fTreeBranchNames;     //
  TString          fTrackCutType;        //
  TString          fTriggerSelection;    //
  TString          fCDBStorage;          //
  Int_t            fMaxTrackSave;        //

  AliTriggerAnalysis fTriggerAnalysis;   //!
  AliAnalysisUtils   fAnalysisUtils;     //!

  TList           *fList;                //!
  TH1             *fHist[kNHist];        //!
  TTree           *fTE;                  //!
  TBits            fIR1InteractionMap;   //!
  TBits            fIR2InteractionMap;   //!
  TBits            fFastOrMap;           //!
  TBits            fFiredChipMap;        //!
  AliESDVertex     fVertexSPD;           //!
  AliESDVertex     fVertexTPC;           //!
  AliESDVertex     fVertexTracks;        //!
  AliTOFHeader     fTOFHeader;           //!
  TClonesArray     fTriggerIRs;          //!
  TString          fFiredTriggerClasses; //!
  TreeData         fTreeData;            //!
  TClonesArray     fTrackData;           //!
  AliESDtrackCuts *fTrackCuts;           //!
  Bool_t           fUseTriggerMask;      //!
  ULong64_t        fClassMask;           //!
  ULong64_t        fClassMaskNext50;     //!
  
  ClassDef(AliAnalysisTaskDG, 1);
} ;

#endif // ALIANALYSISTASKDG_H