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

    embInitPV("kweblogo", argc, argv, "KBWS", "1.0.8");

    struct soap soap;
    struct ns1__weblogoInputParams params;
    char* jobid;
    char* result;

    AjPSeqall  seqall;
    AjPSeq     seq;
    AjPStr     substr;
    AjPStr     inseq = NULL;
    AjPStr     format;

    format =      ajAcdGetString("format");
    seqall = ajAcdGetSeqall("seqall");
    params.format = ajCharNewS(format);

    AjPStr     tmp = NULL;
    AjPStr     tmpFileName = NULL;
    AjPSeqout  fil_file;
    AjPStr     line = NULL; /* if "AjPStr line; -> ajReadline is not success!" */
    AjPStr sizestr = NULL;
    ajint thissize;

    ajint      nb = 0;
    AjBool     are_prot = ajFalse;
    ajint      size = 0;
    AjPFile    infile;

    AjPFile outfile;
    AjPStr filename;

    filename = ajAcdGetString("filename");

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
    if ( soap_call_ns1__runWeblogo( &soap, NULL, NULL, in0, &params, &jobid ) == SOAP_OK ) {
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
      outfile = ajFileNewOutNameS(filename);

      if(!outfile)
        {
          ajFmtError("File open error\n");
          embExitBad();
        }

      if(!gHttpGetBinC(result, &outfile))
        {
          ajFmtError("File downloading error\n");
          embExitBad();
        }
    } else {
      soap_print_fault(&soap, stderr);
    }

    ajSysFileUnlinkS(tmpFileName);

    soap_destroy(&soap);
    soap_end(&soap);
    soap_done(&soap);

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
