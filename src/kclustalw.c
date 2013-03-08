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
  // initialize EMBASSY info
  embInitPV("kclustalw", argc, argv, "KBWS", "1.0.9");

  struct soap soap;
  struct ns1__clustalwInputParams params;
  char* jobid;
  char* result;

  AjPSeqall  seqall;
  AjPSeq     seq;
  AjPFile    outf;
  AjPFile    outf_dnd;
  AjPStr     substr;
  AjPStr     inseq = NULL;

  seqall=   ajAcdGetSeqall("seqall");
  outf=     ajAcdGetOutfile("outfile");
  outf_dnd= ajAcdGetOutfile("dndoutfile");

  params.alignment=  ajCharNewS(ajAcdGetString("alignment"));
  params.output=     ajCharNewS(ajAcdGetString("outformat"));
  params.matrix=     ajCharNewS(ajAcdGetString("matrix"));
  params.outorder=   ajCharNewS(ajAcdGetString("outorder"));
  params.ktup=       ajAcdGetInt("ktup");
  params.window=     ajAcdGetInt("window");
  params.gapopen=    ajAcdGetInt("gapopen");
  params.gapext=     ajAcdGetFloat("gapext");
  params.gapdist=    ajAcdGetInt("gapdist");
  params.pairgap=    ajAcdGetInt("pairgap");;
  params.topdiags=   ajAcdGetInt("topdiags");
  params.score=      ajCharNewS(ajAcdGetString("score"));
  params.outputtree= ajCharNewS(ajAcdGetString("outputtree"));
  params.clustering= ajCharNewS(ajAcdGetString("clustering"););
  params.numiter=    ajAcdGetInt("numiter");
  params.iteration = ajCharNewS(ajAcdGetString("iteration"));

  if (ajAcdGetBoolean("endgaps")) {
    params.endgaps= xsd__boolean__true_;
  } else {
    params.endgaps= xsd__boolean__false_;
  }
  if (ajAcdGetBoolean("tossgaps")) {
    params.tossgaps= xsd__boolean__true_;
  } else {
    params.tossgaps= xsd__boolean__false_;
  }
  if (ajAcdGetBoolean("kimura")) {
    params.kimura= xsd__boolean__true_;
  } else {
    params.kimura= xsd__boolean__false_;
  }
  if (ajAcdGetBoolean("tree")) {
    params.tree= xsd__boolean__true_;
  } else {
    params.tree= xsd__boolean__false_;
  }
  if (ajAcdGetBoolean("quicktree")) {
    params.quicktree= xsd__boolean__true_;
  } else {
    params.quicktree= xsd__boolean__false_;
  }
  if (ajAcdGetBoolean("showalign")) {
    params.align= xsd__boolean__true_;
  } else {
    params.align= xsd__boolean__false_;
  }

  AjPStr tmp= NULL;
  AjPStr tmpFileName= NULL;
  AjPSeqout fil_file;
  AjPStr line= NULL; /* if "AjPStr line; -> ajReadline is not success!" */
  AjPStr sizestr= NULL;
  ajint thissize;

  ajint nb= 0;
  AjBool are_prot= ajFalse;
  ajint size= 0;
  AjPFile infile;

  tmp= ajStrNewC("fasta");

  fil_file= ajSeqoutNew();
  tmpFileName= getUniqueFileName();

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
  in0= ajCharNewS(inseq);
  if (soap_call_ns1__runClustalw(&soap, NULL, NULL, in0, &params, &jobid) == SOAP_OK) {
  } else {
    soap_print_fault(&soap, stderr);
  }

  int check= 0;
  while (check == 0) {
    if (soap_call_ns1__checkStatus(&soap, NULL, NULL, jobid,  &check) == SOAP_OK) {
    } else {
      soap_print_fault(&soap, stderr);
    }
    sleep(3);
  }

  if(soap_call_ns1__getMultiResult(&soap, NULL, NULL, jobid, "out", &result) == SOAP_OK) {
    substr= ajStrNewC(result);
    fprintf(stdout, "%s\n", ajStrGetPtr(substr));
  } else {
    soap_print_fault(&soap, stderr);
  }

  if(soap_call_ns1__getMultiResult(&soap, NULL, NULL, jobid, "aln", &result) == SOAP_OK) {
    substr= ajStrNewC(result);
    ajFmtPrintF(outf,"%S\n",substr);
  } else {
    soap_print_fault(&soap, stderr); 
  }

  if(soap_call_ns1__getMultiResult(&soap, NULL, NULL, jobid, "dnd", &result) == SOAP_OK) {
    substr= ajStrNewC(result);
    ajFmtPrintF(outf_dnd, "%S\n", substr);
  } else {
    soap_print_fault(&soap, stderr); 
  }

  ajSysFileUnlinkS(tmpFileName);

  soap_destroy(&soap);
  soap_end(&soap);
  soap_done(&soap);

  ajFileClose(&outf_dnd);

  ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&substr);

  embExit();

  return 0;
}

static AjPStr getUniqueFileName(void) {
  static char ext[2]= "A";
  AjPStr filename= NULL;

  ajFmtPrintS(&filename, "%08d%s",getpid(), ext);

  if(++ext[0] > 'Z') {
    ext[0]= 'A';
  }

  return filename;
}
