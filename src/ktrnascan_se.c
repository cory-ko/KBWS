#include <stdio.h>
#include <stdlib.h>

#include "soapH.h"
#include "KBWSSoapBinding.nsmap"

#include "emboss.h"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

int main(int argc, char **argv) {

    embInitPV("ktrnascan_se", argc, argv, "KBWS", "1.0.3");

    struct soap soap;
    struct ns1__tRNAscanInputParams params;
    char* jobid;
    char* result;

    AjPSeqall  seqall;
    AjPSeq     seq;
    AjPFile    outf;
    AjPStr     substr;
    AjPStr     inseq = NULL;
    AjPStr     title;
    AjPStr     mode;
    AjPStr     source;
    AjPStr     gcode;
    AjBool     pesudogene;
    AjBool     origin;
    AjBool     ace;
    AjBool     codons;
    AjBool     fpos;
    AjBool     breakdown;
    AjPStr     covescore;
    AjPStr     euparams;
    AjPStr     euscore;

    title      = ajAcdGetString("title");
    mode       = ajAcdGetString("mode");
    source     = ajAcdGetString("source");
    gcode      = ajAcdGetString("gcode");
    pesudogene = ajAcdGetBoolean("pesudogene");
    origin     = ajAcdGetBoolean("origin");
    ace        = ajAcdGetBoolean("ace");
    codons     = ajAcdGetBoolean("codons");
    fpos       = ajAcdGetBoolean("fpos");
    breakdown  = ajAcdGetBoolean("breakdown");
    covescore  = ajAcdGetString("covescore");
    euparams   = ajAcdGetString("euparams");
    euscore    = ajAcdGetString("euscore");

    seqall = ajAcdGetSeqall("seqall");
    outf   = ajAcdGetOutfile("outfile");

    params.title  = ajCharNewS(title);
    params.mode   = ajCharNewS(mode);
    params.source = ajCharNewS(source);
    params.gcode  = ajCharNewS(gcode);
    if (pesudogene) {
      params.pesudogene = xsd__boolean__true_;
    } else {
      params.pesudogene = xsd__boolean__false_;
    }
    if (origin) {
      params.origin = xsd__boolean__true_;
    } else {
      params.origin = xsd__boolean__false_;
    }
    if (ace) {
      params.ace = xsd__boolean__true_;
    } else {
      params.ace = xsd__boolean__false_;
    }
    if (codons) {
      params.codons = xsd__boolean__true_;
    } else {
      params.codons = xsd__boolean__false_;
    }
    if (fpos) {
      params.fpos = xsd__boolean__true_;
    } else {
      params.fpos = xsd__boolean__false_;
    }
    if (breakdown) {
      params.breakdown = xsd__boolean__true_;
    } else {
      params.breakdown = xsd__boolean__false_;
    }
    params.covescore = ajCharNewS(covescore);
    params.euparams  = ajCharNewS(euparams);
    params.euscore   = ajCharNewS(euscore);


    while (ajSeqallNext(seqall, &seq)) {

      soap_init(&soap);

      inseq = NULL;
      ajStrAppendC(&inseq,">");
      ajStrAppendS(&inseq,ajSeqGetNameS(seq));
      ajStrAppendC(&inseq,"\n");
      ajStrAppendS(&inseq,ajSeqGetSeqS(seq));
    
      char* in0;
      in0 = ajCharNewS(inseq);
      if ( soap_call_ns1__runtRNAscan( &soap, NULL, NULL, in0, &params, &jobid ) == SOAP_OK ) {
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

      if(soap_call_ns1__getResult( &soap, NULL, NULL, jobid,  &result )== SOAP_OK) {
        substr = ajStrNewC(result);
        ajFmtPrintF(outf,"%S\n",substr);
      } else {
        soap_print_fault(&soap, stderr); 
      }

    }

    ajFileClose(&outf);

    ajSeqallDel(&seqall);
    ajSeqDel(&seq);
    ajStrDel(&substr);

    embExit();

    return 0;
}
