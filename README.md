## AlphaADD

This program allows you to quickly add transparency to any 24 bit png. You can specify a color key (default 0,0,0) (black) or a color channel to use to create the transparency information. Super easy to use. 

 Usage: AlphaAdd.exe <infile>.png <option(s)>
 Options:
 -h,--help Show this help message
 -k <num,num,num> Set a new color key for all RGB Channels, otherwise 255,255,255 (black) is used.
 -c <channel> Force R/r or G/g or B/b for SINGLE channel to use for alpha, otherwise the default or selected key is used.
 -f <outfile.png> Specify out filename, otherwise it's <infile>_alpha.png

## Installation

Download, compile and run, or just use the included exe.


## License

This uses the absolutely free license from http://unlicense.org/, you can do anything you want with this code, although if you make it better, I'd appreciate you giving me the changes so I can make my code better for everyone.