#include <stdio.h>
#include <stdlib.h>

#include "soapH.h"
#include "KBWSSoapBinding.nsmap"

#include "emboss.h"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

int main(int argc, char **argv) {

    embInitPV("krnafold", argc, argv, "KBWS", "1.0.4");

    struct soap soap;
    struct ns1__RNAfoldInputParams params;
    char* jobid;
    char* result;

    AjPSeqall seqall;
    AjPSeq    seq;
    AjPFile   outf;
    AjPFile   outf_ct;
    AjPFile   outf_vienna;
    AjPStr    substr;
    AjPStr    inseq = NULL;

    seqall       = ajAcdGetSeqall("seqall");
    outf         = ajAcdGetOutfile("outfile");
    outf_ct      = ajAcdGetOutfile("ctfile");
    outf_vienna  = ajAcdGetOutfile("viennafile");

    AjPStr    method;
    AjBool    noclosegu;
    AjBool    nolp;
    AjPStr    dangling;
    AjPStr    param;
    ajint     tmp;
    AjBool    circ;

    method    = ajAcdGetString("method");
    noclosegu = ajAcdGetBoolean("noclosegu");
    nolp      = ajAcdGetBoolean("nolp");
    dangling  = ajAcdGetString("dangling");
    param     = ajAcdGetString("param");
    tmp       = ajAcdGetInt("tmp");
    circ      = ajAcdGetBoolean("circ");

    params.method      = ajCharNewS(method);
    if (noclosegu) {
      params.noclosegu = xsd__boolean__true_;
    } else {
      params.noclosegu = xsd__boolean__false_;
    }
    if (nolp) {
      params.nolp      = xsd__boolean__true_;
    } else {
      params.nolp      = xsd__boolean__false_;
    }
    params.dangling    = ajCharNewS(dangling);
    params.param       = ajCharNewS(param);
    params.tmp         = tmp;
    if (circ) {
      params.circ      = xsd__boolean__true_;
    } else {
      params.circ      = xsd__boolean__false_;
    }


    while (ajSeqallNext(seqall, &seq)) {
      inseq = NULL;
      ajStrAppendC(&inseq,">");
      ajStrAppendS(&inseq,ajSeqGetNameS(seq));
      ajStrAppendC(&inseq,"\n");
      ajStrAppendS(&inseq,ajSeqGetSeqS(seq));

      soap_init(&soap);

      char* in0;
      in0 = ajCharNewS(inseq);
      if ( soap_call_ns1__runRNAfold( &soap, NULL, NULL, in0, &params, &jobid ) == SOAP_OK ) {
          fprintf(stderr,"Jobid: %s\n",jobid);
      } else {
          soap_print_fault(&soap, stderr);
      }

      int check = 0;
      while ( check == 0 ) {
          if ( soap_call_ns1__checkStatus( &soap, NULL, NULL, jobid,  &check ) == SOAP_OK ) {
              fprintf(stderr,"*");
          } else {
              soap_print_fault(&soap, stderr);
          }
          sleep(3);
      }

      fprintf(stderr,"\n");

      char* type;
      type = "out";
      if(soap_call_ns1__getMultiResult( &soap, NULL, NULL, jobid, type, &result )== SOAP_OK) {
	substr = ajStrNewC(result);
	ajFmtPrintF(outf,"%S\n",substr);
      } else {
	soap_print_fault(&soap, stderr); 
      }

      
      type = "ct";
      if(soap_call_ns1__getMultiResult( &soap, NULL, NULL, jobid, type, &result )== SOAP_OK) {
	substr = ajStrNewC(result);
        ajFmtPrintF(outf_ct,"%S\n",substr);
      } else {
	soap_print_fault(&soap, stderr); 
      }

      type = "vienna";
      if(soap_call_ns1__getMultiResult( &soap, NULL, NULL, jobid, type, &result )== SOAP_OK) {
	substr = ajStrNewC(result);
        ajFmtPrintF(outf_vienna,"%S\n",substr);
      } else {
	soap_print_fault(&soap, stderr); 
      }

      soap_destroy(&soap);
      soap_end(&soap);
      soap_done(&soap);
      
    }

    ajFileClose(&outf);
    
    ajSeqallDel(&seqall);
    ajSeqDel(&seq);
    ajStrDel(&substr);
    
    embExit();
    
    return 0;
}
