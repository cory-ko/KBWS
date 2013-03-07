#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "KBWSSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

int main(int argc, char **argv) {
  // initialize EMBASSY info
  embInitPV("kblast", argc, argv, "KBWS", "1.0.8");

  // soap driver and parameter object
  struct soap soap;
  struct ns1__blastInputParams params;

  char* jobid;

  AjPSeqall seqall; // input sequence
  AjPFile   outf; // outfile

  AjPSeq seq;
  AjPStr inseq= NULL;

  AjPStr substr;
    
  // get input/output info
  seqall= ajAcdGetSeqall("seqall");
  outf= ajAcdGetOutfile("outfile");

  // set parameters
  params.p= ajCharNewS(ajAcdGetString("program"));
  params.d= ajCharNewS(ajAcdGetString("database"));
  params.server= ajCharNewS(ajAcdGetString("server"));
  params.m= ajCharNewS(ajAcdGetString("format"));
  params.e= ajCharNewS(ajAcdGetString("eval"));
  params.F= ajCharNewS(ajAcdGetString("qfilter"));
  params.G= ajAcdGetInt("opengap");
  params.E= ajAcdGetInt("extendgap");
  params.X= ajCharNewS(ajAcdGetString("dropoff"));
  params.q= ajAcdGetInt("penalty");
  params.r= ajAcdGetInt("reward");
  params.v= ajAcdGetInt("numdescriptions");
  params.b= ajAcdGetInt("numalignments");
  params.f= ajAcdGetInt("threshold");
  params.M= ajCharNewS(ajAcdGetString("matrix"));
  params.W= ajAcdGetInt("wordsize");
  params.z= ajAcdGetFloat("dbsize");
  params.K= ajAcdGetInt("k");
  params.Y= ajAcdGetFloat("searchsp");
  if (ajAcdGetBoolean("g")) {
    params.g= xsd__boolean__true_;
  } else {
    params.g= xsd__boolean__false_;
  }

  while (ajSeqallNext(seqall, &seq)) {
    // initialize
    soap_init(&soap);
    inseq= NULL;

    // convert sequence data to EMBOSS string as fasta format
    ajStrAppendC(&inseq, ">");
    ajStrAppendS(&inseq, ajSeqGetNameS(seq));
    ajStrAppendC(&inseq, "\n");
    ajStrAppendS(&inseq, ajSeqGetSeqS(seq));

    // convert EMBOSS string to char* in C
    char* in0;
    in0= ajCharNewS(inseq);

    // submit query via SOAP and get job ID
    if (soap_call_ns1__runBlast(&soap, NULL, NULL, in0, &params, &jobid) == SOAP_OK) {
    } else {
      soap_print_fault(&soap, stderr);
    }

    // polling
    int check= 0;
    while (check == 0) {
      if (soap_call_ns1__checkStatus(&soap, NULL, NULL, jobid,  &check) == SOAP_OK) {
      } else {
	soap_print_fault(&soap, stderr);
      }
      sleep(3);
    }

    // get result
    char* result;
    if (soap_call_ns1__getResult(&soap, NULL, NULL, jobid,  &result) == SOAP_OK) {
      // convert result from C char* to EMBOSS string object
      substr= ajStrNewC(result);

      // output result (EMBOSS string) to file or STDOUT via EMBOSS
      ajFmtPrintF(outf, "%S\n", substr);
    } else {
      soap_print_fault(&soap, stderr);
    }

  }

  // destruct SOAP driver
  soap_destroy(&soap);
  soap_end(&soap);
  soap_done(&soap);

  // write output file and destruct outfile object
  ajFileClose(&outf);

  // destruct EMBOSS objects
  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&substr);

  embExit();

  return 0;
}
