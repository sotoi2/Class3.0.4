# invoke SourceDir generated makefile for tivac_tirtos.pem4f
tivac_tirtos.pem4f: .libraries,tivac_tirtos.pem4f
.libraries,tivac_tirtos.pem4f: package/cfg/tivac_tirtos_pem4f.xdl
	$(MAKE) -f C:\Users\oit\Downloads\TIRTOS_Project1\TIRTOS_Project/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\oit\Downloads\TIRTOS_Project1\TIRTOS_Project/src/makefile.libs clean

