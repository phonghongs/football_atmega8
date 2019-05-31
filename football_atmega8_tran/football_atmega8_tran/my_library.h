#define cbi(port,bit) \
(port) &= ~(1 << (bit))
#define sbi(port,bit) \
(port) |= (1 << (bit))
#define bit_is_set(sfr,bit) \
(_SFR_BYTE(sfr) & _BV(bit))
#define bit_is_clear(sfr,bit) \
(!(_SFR_BYTE(sfr) & _BV(bit)))
