// vsmul: Signed saturating and rounding fractional multiply
VRM xrm = P_.VU.get_vround_mode();
int64_t int_max = (uint64_t(1) << (P_.VU.vsew - 1)) - 1;
int64_t int_min = - (1 << (P_.VU.vsew - 1));
int64_t sign_mask = uint64_t(1) << (P_.VU.vsew - 1);

VI_VV_LOOP
({
  int64_t vs1_sign;
  int64_t vs2_sign;
  int64_t result_sign;

  vs1_sign = vs1 & sign_mask;
  vs2_sign = vs2 & sign_mask;
  bool overflow = vs1 == vs2 && vs1 == int_min;

  int128_t result = (int128_t)vs1 * (int128_t)vs2;
  result_sign = (vs1_sign ^ vs2_sign) & sign_mask;

  // rounding
  INT_ROUNDING(result, xrm, sew - 1);
  // remove guard bits
  result = result >> (sew - 1);

  // saturation
  if (overflow) {
    result = int_max;
    P_.VU.vxsat = 1;
  }

  vd = result;
})
