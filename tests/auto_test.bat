@for %%f in (*.csc) do @echo Testing %%f...&&..\cs.exe --wait-before-exit %%f
