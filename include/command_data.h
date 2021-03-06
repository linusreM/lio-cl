#include "liomatrix.h"

const lio_row_data_t matrix_data[17] = {
    {{  //Test pattern
        0b01111110,
        0b11111111,
        0b11100111,
        0b11100111,
        0b11100111,
        0b11100111,
        0b11111111,
        0b01111110
    }},
    {{   //1
        0b00011100,
        0b00111100,
        0b01111100,
        0b00011100,
        0b00011100,
        0b00011100,
        0b00111110,
        0b00111110
    }},
    {{   //2
        0b01111110,
        0b11111111,
        0b11100111,
        0b00011100,
        0b00111000,
        0b01110000,
        0b11111111,
        0b11111111
    }},
    {{   //3
        0b11111110,
        0b11111111,
        0b00000111,
        0b00011110,
        0b00011110,
        0b00000111,
        0b11111111,
        0b11111110
    }},
    {{   //4
        0b11101110,
        0b11101110,
        0b11101110,
        0b11111110,
        0b11111110,
        0b00001110,
        0b00001110,
        0b00001110
    }},
    {{   //5
        0b11111111,
        0b11111111,
        0b11100000,
        0b11111110,
        0b11111111,
        0b00000111,
        0b11111111,
        0b11111110
    }},
    {{   //6
        0b00001110,
        0b00011100,
        0b00111000,
        0b01111110,
        0b11111111,
        0b11100111,
        0b11111111,
        0b01111110
    }},
    {{   //7
        0b11111111,
        0b11111111,
        0b00000111,
        0b00000111,
        0b00001110,
        0b00011100,
        0b00111000,
        0b01110000
     }},
    {{   //8
        0b01111110,
        0b11111111,
        0b11100111,
        0b01111110,
        0b11100111,
        0b11100111,
        0b11111111,
        0b01111110
     
    }},
    {{  //9
        0b01111110,
        0b11111111,
        0b11100111,
        0b11111111,
        0b01111111,
        0b00000111,
        0b11111111,
        0b11111110
    }},
    {{  //A
        0b01111110,
        0b11111111,
        0b11100111,
        0b11111111,
        0b11111111,
        0b11100111,
        0b11100111,
        0b11100111
    }},
    {{  //B
        0b11111100,
        0b11111110,
        0b11100111,
        0b11111110,
        0b11111100,
        0b11111110,
        0b11100111,
        0b11111110
    }},
    {{   //C
        0b00111100,
        0b01111110,
        0b11100111,
        0b11100000,
        0b11100000,
        0b11100111,
        0b01111110,
        0b00111100
    }},
    {{   //D
        0b11111100,
        0b11111110,
        0b11100111,
        0b11100111,
        0b11100111,
        0b11100111,
        0b11111110,
        0b11111100
    }},
    {{   //*
        0b00111100,
        0b01111110,
        0b11111111,
        0b11111111,
        0b11111111,
        0b11111111,
        0b01111110,
        0b00111100
    }},
    {{   //#
        0b01100110,
        0b11111111,
        0b11111111,
        0b01100110,
        0b01100110,
        0b11111111,
        0b11111111,
        0b01100110
    }},
    {{   //#
        0x55,
        0x55,
        0x55,
        0x55,
        0x55,
        0x55,
        0x55,
        0x55
    }}
    };