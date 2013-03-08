#include <stdio.h>
#include <stdlib.h>

#include "soapH.h"
#include "KBWSSoapBinding.nsmap"

#include "emboss.h"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

int main(int argc, char **argv) {
  // initialize EMBASSY info
  embInitPV("ktrnascan_se", argc, argv, "KBWS", "1.0.9");

  struct soap soap;
  struct ns1__tRNAscanInputParams params;

  char* jobid;

  AjPSeqall seqall;
  AjPSeq seq;
  AjPFile outf;
  AjPStr substr;
  AjPStr inseq = NULL;

  seqall= ajAcdGetSeqall("seqall");
  outf= ajAcdGetOutfile("outfile");

  params.title=     ajCharNewS(ajAcdGetString("title"));
  params.mode=      ajCharNewS(ajAcdGetString("mode"));
  params.source=    ajCharNewS(ajAcdGetString("source"));
  params.gcode=     ajCharNewS(ajAcdGetString("gcode"));
  params.covescore= ajCharNewS(ajAcdGetString("covescore"));
  params.euparams=  ajCharNewS(ajAcdGetString("euparams"));
  params.euscore=   ajCharNewS(ajAcdGetString("euscore"));

  if (ajAcdGetBoolean("pesudogene")) {
    params.pesudogene= xsd__boolean__true_;
  } else {
    params.pesudogene= xsd__boolean__false_;
  }

  if (ajAcdGetBoolean("origin")) {
    params.origin= xsd__boolean__true_;
  } else {
    params.origin= xsd__boolean__false_;
  }

  if (ajAcdGetBoolean("ace")) {
    params.ace= xsd__boolean__true_;
  } else {
    params.ace= xsd__boolean__false_;
  }

  if (ajAcdGetBoolean("codons")) {
    params.codons= xsd__boolean__true_;
  } else {
    params.codons= xsd__boolean__false_;
  }

  if (ajAcdGetBoolean("fpos")) {
    params.fpos= xsd__boolean__true_;
  } else {
    params.fpos = xsd__boolean__false_;
  }

  if (ajAcdGetBoolean("breakdown")) {
    params.breakdown= xsd__boolean__true_;
  } else {
    params.breakdown= xsd__boolean__false_;
  }

  while (ajSeqallNext(seqall, &seq)) {
    soap_init(&soap);

    inseq= NULL;
    ajStrAppendC(&inseq, ">");
    ajStrAppendS(&inseq, ajSeqGetNameS(seq));
    ajStrAppendC(&inseq, "\n");
    ajStrAppendS(&inseq, ajSeqGetSeqS(seq));
    
    char* in0;
    in0= ajCharNewS(inseq);
    if (soap_call_ns1__runtRNAscan(&soap, NULL, NULL, in0, &params, &jobid) == SOAP_OK) {
    } else {
      soap_print_fault(&soap, stderr);
    }

    int check= 0;
    while (check == 0) {
      if (soap_call_ns1__checkStatus(&soap, NULL, NULL, jobid, &check) == SOAP_OK) {
      } else {
	soap_print_fault(&soap, stderr);
      }
      sleep(3);
    }

    char* result;
    if(soap_call_ns1__getResult(&soap, NULL, NULL, jobid, &result) == SOAP_OK) {
      substr= ajStrNewC(result);
      ajFmtPrintF(outf, "%S\n", substr);
    } else {
      soap_print_fault(&soap, stderr); 
    }
  }

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
