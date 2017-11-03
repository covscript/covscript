@for %%f in (*.csc) do @echo Testing %%f...&&cs --wait-before-exit %%f
