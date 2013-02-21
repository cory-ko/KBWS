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
  embInitPV("kcentroidfold", argc, argv, "KBWS", "1.0.8");

  // soap driver and parameter object
  struct soap soap;
  struct ns1__centroidfoldInputParams params;

  char* jobid;

  AjPSeqall seqall; // input sequence
  AjPFile   outf; // outfile
  AjPStr    goutfile; // graph file name
  AjPFile   goutf; // graph file handle

  AjPSeq     seq;
  AjPStr     inseq= NULL;

  AjPStr     substr;

  AjPStr     engine; // CONTRAfold, McCaskill, pfold or AUX
  ajint      gamma;

  // get input/output info
  seqall= ajAcdGetSeqall("seqall");
  outf= ajAcdGetOutfile("outfile");
  goutfile= ajAcdGetString("goutfile");

  // get parameters
  engine= ajAcdGetString("engine");
  gamma=  ajAcdGetInt("gamma");

  // set parameters
  params.model= ajCharNewS(engine);
  params.gamma= gamma;

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
    if (soap_call_ns1__runCentroidfold(&soap, NULL, NULL, in0, &params, &jobid) == SOAP_OK) {
      // warn user's job ID
      fprintf(stderr, "Jobid: %s\n", jobid);
    } else {
      soap_print_fault(&soap, stderr);
    }

    // polling
    int check = 0;
    while (check == 0) {
      if (soap_call_ns1__checkStatus(&soap, NULL, NULL, jobid, &check) == SOAP_OK) {
	// progress bar
	fprintf(stderr, "*");
      } else {
	soap_print_fault(&soap, stderr);
      }
      sleep(3);
    }

    fprintf(stderr, "\n");

    // get result (sequence alignment text data)
    char* result;
    if(soap_call_ns1__getMultiResult(&soap, NULL, NULL, jobid, "out", &result) == SOAP_OK) {
      // convert result from C char* to EMBOSS string object
      substr= ajStrNewC(result);

      // output result (EMBOSS string) to file or STDOUT via EMBOSS
      ajFmtPrintF(outf, "%S\n", substr);
    } else {
      soap_print_fault(&soap, stderr); 
    }

    // get result (image file)
    char* image_url;
    if(soap_call_ns1__getMultiResult(&soap, NULL, NULL, jobid, "png", &image_url) == SOAP_OK) {
      goutf= ajFileNewOutNameS(goutfile);
      
      if (!goutf) {
	// can not open image output file
	ajFmtError("Problem writing out image file");
	embExitBad();
      }

      if (!gHttpGetBinC(image_url, &goutf)) {
	// can not download image file
	ajFmtError("Problem downloading image file");
	embExitBad();
      }
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

  // destruct EMBOSS object
  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&substr);
  ajStrDel(&engine);

  // exit
  embExit();
    
  return 0;
}
