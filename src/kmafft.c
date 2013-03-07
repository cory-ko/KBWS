#include <stdio.h>
#include <stdlib.h>

#include "soapH.h"
#include "KBWSSoapBinding.nsmap"

#include "emboss.h"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

static AjPStr getUniqueFileName(void);
    int main(int argc, char **argv) {

    embInitPV("kmafft", argc, argv, "KBWS", "1.0.8");

    struct soap soap;
    struct ns1__mafftInputParams params;
    char* jobid;
    char* result;

    AjPSeqall  seqall;
    AjPSeq     seq;
    AjPFile    outf;
    AjPStr     substr;
    AjPStr     inseq = NULL;
    AjPStr     strategy;
    AjPStr     outorder;
    float      opengap;
    float      ep;
    AjPStr     scorematrix;
    AjBool     homologs;
    AjBool     showhomologs;
    float      threshold;
    AjPStr     referenceseq;
    AjPStr     harrplot;

    strategy     =      ajAcdGetString("strategy");
    outorder     =      ajAcdGetString("outorder");
    opengap      =      ajAcdGetFloat("opengap");
    ep           =      ajAcdGetFloat("ep");
    scorematrix  =      ajAcdGetString("scorematrix");
    homologs     =      ajAcdGetBoolean("homologs");
    showhomologs =      ajAcdGetBoolean("showhomologs");
    threshold    =      ajAcdGetFloat("threshold");
    referenceseq =      ajAcdGetString("referenceseq");
    harrplot     =      ajAcdGetString("harrplot");

    seqall = ajAcdGetSeqall("seqall");
    outf   = ajAcdGetOutfile("outfile");

    params.strategy = ajCharNewS(strategy);
    params.outorder = ajCharNewS(outorder);
    params.op = opengap;
    params.ep = ep;
    params.scorematrix = ajCharNewS(scorematrix);
    if (homologs) {
      params.homologs = xsd__boolean__true_;
    } else {
      params.homologs = xsd__boolean__false_;
    }
    if (showhomologs) {
      params.showhomologs = xsd__boolean__true_;
    } else {
      params.showhomologs = xsd__boolean__false_;
    }
    params.threshold = threshold;
    params.referenceseq = ajCharNewS(referenceseq);
    params.harrplot = ajCharNewS(harrplot);

    AjPStr     tmp         = NULL;
    AjPStr     tmpFileName = NULL;
    AjPSeqout  fil_file;
    AjPStr     line        = NULL; /* if "AjPStr line; -> ajReadline is not success!" */
    AjPStr     sizestr     = NULL;
    ajint      thissize;

    ajint      nb       = 0;
    AjBool     are_prot = ajFalse;
    ajint      size     = 0;
    AjPFile    infile;

    tmp = ajStrNewC("fasta");

    fil_file = ajSeqoutNew();
    tmpFileName = getUniqueFileName();

    if( !ajSeqoutOpenFilename(fil_file, tmpFileName) ) {
        embExitBad();
    }

    ajSeqoutSetFormatS(fil_file, tmp);

    while (ajSeqallNext(seqall, &seq)) {
      if (!nb) {
        are_prot  = ajSeqIsProt(seq);
    }
      ajSeqoutWriteSeq(fil_file, seq);
      ++nb;
    }
    ajSeqoutClose(fil_file);
    ajSeqoutDel(&fil_file);

    if (nb < 2) {
        ajFatal("Multiple alignments need at least two sequences");
    }

    infile = ajFileNewInNameS(tmpFileName);

    while (ajReadline(infile, &line)) {
      ajStrAppendS(&inseq,line);
      ajStrAppendC(&inseq,"\n");
    }

    soap_init(&soap);

    char* in0;
    in0 = ajCharNewS(inseq);
    if ( soap_call_ns1__runMafft( &soap, NULL, NULL, in0, &params, &jobid ) == SOAP_OK ) {
    } else {
      soap_print_fault(&soap, stderr);
    }

    int check = 0;
    while ( check == 0 ) {
      if ( soap_call_ns1__checkStatus( &soap, NULL, NULL, jobid,  &check ) == SOAP_OK ) {
      } else {
        soap_print_fault(&soap, stderr);
      }
      sleep(3);
    }

    if ( soap_call_ns1__getResult( &soap, NULL, NULL, jobid,  &result ) == SOAP_OK ) {
      substr = ajStrNewC(result);
      ajFmtPrintF(outf,"%S\n",substr);
    } else {
      soap_print_fault(&soap, stderr);
    }

    ajSysFileUnlinkS(tmpFileName);

    soap_destroy(&soap);
    soap_end(&soap);
    soap_done(&soap);

    ajFileClose(&outf);

    ajSeqallDel(&seqall);
    ajSeqDel(&seq);
    ajStrDel(&substr);

    embExit();

    return 0;
}

static AjPStr getUniqueFileName(void) {
  static char ext[2] = "A";
  AjPStr filename    = NULL;

  ajFmtPrintS(&filename, "%08d%s",getpid(), ext);

  if( ++ext[0] > 'Z' ) {
    ext[0] = 'A';
  }

  return filename;
}
