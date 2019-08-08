 --- Mega2560 Signal Analyzer Version 2.1 --- 

This is a simple program that repeatedly extracts the an analog signal and finds its strongest frequencies. This version only supports the largest 2 frequencies.
Due to the fixed point multiplication and memory limitations the minimum detectable amplitude is 0.2V.
The higher the amplitude of the signal, the more consistant and precise the analysis is.


 -- Defaults -- 
  analog input = 0

  recording length = 1024
  result length = 150

  reference level = 127

  continuous = false
  detail = 1
  estimate = true

-- Commands (non-case sensitive) --
T - Top Menu only
A - During Analysis only
G - Global
-----------------------------------

S<#>: 	Starts analysis
	(optional) sets analog input to # (T)
A<#>: 	Displays analog input
	(optional) sets analog input to # (G)

L<#>: 	Displays length of recording, 
	(optional) sets length to 2^# (T)
      	1 <= # <= 10
RL<#>: 	Displays Result Length, 
	(optional) sets length of result to # (T)
       	1 <= # <= 750 (recomended max = 370)


T: 	Triggers a signal reading (A)

C: 	Toggles continuous reading (G)
RT: 	Resets timer (A)
RC: 	Resets timer and toggles continuous reading (A)

CLRR:	Clears the results (G)

E:	Exits analysis (A)

D<#>: 	Sets level of detail for each input (G)
      	# = 0: No output
      	# = 1: List Frequencies
      	# = 2: Lists Analysis Details

EST: 	Toggles estimation (Estimation results in less consistant results but is more acurate on average) (G)

REF<#>:	Displays reference level, 
	(optional) Sets the reference level to # or to the current detected analog input if # = 'D' (G)


PSIG: 	Prints analog signal (A)
PSIGO:	Prints analog signal without time (A)
PFT:	Prints Fourier Transform (A)
PFTO:	Prints Fourier Transform without time (A)
PR: 	Prints results(G)



