#include <stdio.h>
#include <stdlib.h>

#include "soapH.h"
#include "KBWSSoapBinding.nsmap"

#include "emboss.h"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

int main(int argc, char **argv) {

    embInitPV("kfetchdata", argc, argv, "KBWS", "1.0.3");

    struct soap soap;
    struct ns1__fetchDataInputParams params;
    char* jobid;
    char* result;

    AjPStr   id;
    AjPFile  outf;
    AjPStr   substr;
    AjPStr   format;
    AjPStr   style;

    id     = ajAcdGetString("id");
    outf   = ajAcdGetOutfile("outfile");
    format = ajAcdGetString("format");
    style  = ajAcdGetString("style");

    params.format = ajCharNewS(format);
    params.style  = ajCharNewS(style);

    soap_init(&soap);

    char* in0;
    in0 = ajCharNewS(id);
    if ( soap_call_ns1__runFetchData( &soap, NULL, NULL, in0, &params, &jobid ) == SOAP_OK ) {
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

    if ( soap_call_ns1__getResult( &soap, NULL, NULL, jobid,  &result ) == SOAP_OK ) {
      substr = ajStrNewC(result);
      ajFmtPrintF(outf,"%S\n",substr);
    } else {
      soap_print_fault(&soap, stderr);
    }


    soap_destroy(&soap);
    soap_end(&soap);
    soap_done(&soap);

    ajStrDel(&id);
    ajFileClose(&outf);
    ajStrDel(&substr);

    embExit();

    return 0;
}
