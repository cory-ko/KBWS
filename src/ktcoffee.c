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

    embInitPV("ktcoffee", argc, argv, "KBWS", "1.0.3");

    AjPSeqall  seqall;
    AjPSeq     seq;
    AjPFile    outf;
    AjPFile    outf_aln;
    AjPFile    outf_dnd;
    AjPStr     substr;
    AjPStr     line = NULL; /* if "AjPStr line; -> ajReadline is not success!" */

    AjPStr     sizestr = NULL;
    ajint      thissize;

    AjPStr     tmp = NULL;
    AjPStr     tmpFileName = NULL;
    AjPSeqout  fil_file;

    ajint      nb = 0;
    AjBool     are_prot = ajFalse;
    ajint      size = 0;
    AjPFile    infile;
    AjPStr     multi = NULL;

    AjPStr     outorder;
    AjPStr     matrix;

    struct soap soap;
    struct ns1__tcoffeeInputParams params;
    char* jobid;
    char* result;

    outorder        = ajAcdGetString("outorder");
    matrix          = ajAcdGetString("matrix");
    params.outorder = ajCharNewS(outorder);
    params.matrix   = ajCharNewS(matrix);

    seqall          = ajAcdGetSeqall("sequence");
    outf            = ajAcdGetOutfile("outfile");
    outf_aln        = ajAcdGetOutfile("alnoutfile");
    outf_dnd        = ajAcdGetOutfile("dndoutfile");

    tmp = ajStrNewC("fasta");

    soap_init(&soap);

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
      ajStrAppendS(&multi,line);
      ajStrAppendC(&multi,"\n");
    }
    
    char* in0;
    in0 = ajCharNewS(multi);
      
    if(soap_call_ns1__runTcoffee( &soap, NULL, NULL, in0, &params, &jobid )== SOAP_OK) {
      fprintf(stderr,"Jobid: %s\n",jobid);
    } else {
      soap_print_fault(&soap, stderr); 
    }

    int check = 0;
    while (check == 0) {
      if(soap_call_ns1__checkStatus( &soap, NULL, NULL, jobid,  &check )== SOAP_OK) {
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

    type = "aln";
    if(soap_call_ns1__getMultiResult( &soap, NULL, NULL, jobid, type, &result )== SOAP_OK) {
      substr = ajStrNewC(result);
      ajFmtPrintF(outf_aln,"%S\n",substr);
    } else {
      soap_print_fault(&soap, stderr); 
    }

    type = "dnd";
    if(soap_call_ns1__getMultiResult( &soap, NULL, NULL, jobid, type, &result )== SOAP_OK) {
      substr = ajStrNewC(result);
      ajFmtPrintF(outf_dnd,"%S\n",substr);
    } else {
      soap_print_fault(&soap, stderr); 
    }
    
    soap_destroy(&soap); 
    soap_end(&soap); 
    soap_done(&soap); 
    
    ajSysFileUnlink(tmpFileName);

    ajFileClose(&outf);
    ajFileClose(&outf_aln);
    ajFileClose(&outf_dnd);
   
    ajSeqallDel(&seqall);
    ajSeqDel(&seq);
    ajStrDel(&substr);

    ajStrDel(&matrix);
    ajStrDel(&outorder);

    ajSeqoutDel(&fil_file);

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

