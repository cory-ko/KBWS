## Process this file with automake to produce Makefile.in

INCLUDES = -I../include -I../../../nucleus -I../../../ajax/pcre \
	-I../../../ajax/expat -I../../../ajax/zlib \
	-I../../../ajax/core -I../../../ajax/graphics \
	-I../../../ajax/ensembl -I../../../ajax/ajaxdb \
	-I../../../ajax/acd -I../../../plplot -I../gsoap

if ISSHARED
if ISAIXIA64
AIX_CFLAGS = -Wl,-bdynamic -Wl,-brtl -L${embprefix}/lib -lnucleus -lacd \
-lajaxdb -lensembl -lajaxg -lajax -leexpat -lepcre -lezlib -leplplot
endif
endif

AM_CFLAGS =  $(AIX_CFLAGS) $(WARN_CFLAGS) $(DEVWARN_CFLAGS)

## To add programs
## Add the program binary name to bin_PROGRAMS
## (using \ as a continuation character for multiple lines)
##
## And add a programname_SOURCES line to define the source files
## to be compiled and linked
##
## make will compile and link the program
## make install will copy the program to the install directory

bin_PROGRAMS = kblast kcentroidfold kclique kclustalw kcontml kdnacomp kdnadist kdnainvar kdnaml kdnamlk kdnapars kdnapenny \
               kdollop kdolpenny kfetchbatch kfetchdata kfitch kgendist kgenemarkhmm kglimmer kkalign kkitsch kmafft kmix \
               kmuscle kneighbor kpenny kphobius kprotdist kprotpars kpsort kpsort2 kpsortb krestml kseqboot kssearch \
               ktcoffee ktrnascan_se kweblogo kwolfpsort krnafold kshowblastdb kpathwayprojector

kblast_SOURCES            = kblast.c
kcentroidfold_SOURCES     = kcentroidfold.c
kclique_SOURCES           = kclique.c
kclustalw_SOURCES         = kclustalw.c
kcontml_SOURCES           = kcontml.c
kdnacomp_SOURCES          = kdnacomp.c
kdnadist_SOURCES          = kdnadist.c
kdnainvar_SOURCES         = kdnainvar.c
kdnaml_SOURCES            = kdnaml.c
kdnamlk_SOURCES           = kdnamlk.c
kdnapars_SOURCES          = kdnapars.c
kdnapenny_SOURCES         = kdnapenny.c
kdollop_SOURCES           = kdollop.c
kdolpenny_SOURCES         = kdolpenny.c
kfetchbatch_SOURCES       = kfetchbatch.c
kfetchdata_SOURCES        = kfetchdata.c
kfitch_SOURCES            = kfitch.c
kgendist_SOURCES          = kgendist.c
kgenemarkhmm_SOURCES      = kgenemarkhmm.c
kglimmer_SOURCES          = kglimmer.c
kkalign_SOURCES           = kkalign.c
kkitsch_SOURCES           = kkitsch.c
kmafft_SOURCES            = kmafft.c
kmix_SOURCES              = kmix.c
kmuscle_SOURCES           = kmuscle.c
kneighbor_SOURCES         = kneighbor.c
kpenny_SOURCES            = kpenny.c
kphobius_SOURCES          = kphobius.c
kprotdist_SOURCES         = kprotdist.c
kprotpars_SOURCES         = kprotpars.c
kpsort_SOURCES            = kpsort.c
kpsort2_SOURCES           = kpsort2.c
kpsortb_SOURCES           = kpsortb.c
krestml_SOURCES           = krestml.c
kseqboot_SOURCES          = kseqboot.c
kssearch_SOURCES          = kssearch.c
ktcoffee_SOURCES          = ktcoffee.c
ktrnascan_se_SOURCES      = ktrnascan_se.c
kweblogo_SOURCES          = kweblogo.c
kwolfpsort_SOURCES        = kwolfpsort.c
krnafold_SOURCES          = krnafold.c
kshowblastdb_SOURCES      = kshowblastdb.c
kpathwayprojector_SOURCES = kpathwayprojector.c

LDADD = -L${embprefix}/lib -lnucleus -lacd -lajaxdb -lensembl -lajaxg \
        -lajax -leexpat -lepcre -lezlib -leplplot $(XLIB)

