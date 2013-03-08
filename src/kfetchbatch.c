]#include <stdio.h>
#include <stdlib.h>

#include "soapH.h"
#include "KBWSSoapBinding.nsmap"

#include "emboss.h"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

int main(int argc, char **argv) {
  // initialize EMBASSY info
  embInitPV("kfetchbatch", argc, argv, "KBWS", "1.0.9");

  struct soap soap;
  struct ns1__fetchBatchInputParams params;
  char* jobid;
  char* result;

  AjPStr   dbname;
  AjPStr   idlist;
  AjPFile  outf;
  AjPStr   substr;
  AjPStr   format;
  AjPStr   style;

  dbname = ajAcdGetString("dbname");
  idlist = ajAcdGetString("idlist");
  outf   = ajAcdGetOutfile("outfile");
  format = ajAcdGetString("format");
  style  = ajAcdGetString("style");

  params.format = ajCharNewS(format);
  params.style  = ajCharNewS(style);

  soap_init(&soap);

  char* in0;
  in0 = ajCharNewS(dbname);
  char* in1;
  in1 = ajCharNewS(idlist);
  if ( soap_call_ns1__runFetchBatch( &soap, NULL, NULL, in0, in1, &params, &jobid ) == SOAP_OK ) {
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


  soap_destroy(&soap);
  soap_end(&soap);
  soap_done(&soap);

  ajStrDel(&idlist);
  ajStrDel(&dbname);
  ajFileClose(&outf);
  ajStrDel(&substr);

  embExit();

  return 0;
 }
