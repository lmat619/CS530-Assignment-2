COPY     START   0                 TEST PROGRAM FOR SIC SOFTWARE   
FIRST    STL     RETADR            SAVE RETURN ADDRESS             
         LDB     LENGTH                                            
CLOOP   +JSUB    ENDFIL            READ INPUT RECORD               
         LDA     LENGTH            TEST FOR EOF (LENGTH = 0        
         COMP   #0                 PLUS EOL OR EOF)                
ENDFIL   COMPR   A,S                                               
        +LDT    #4096                                              
LENGTH   RESW    1                                                 
         END     FIRST                                             