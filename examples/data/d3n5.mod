var x0 >= -0.57735 <= 0.57735;
var x1 >= -0.57735 <= 0.57735;
var x2 >= -0.57735 <= 0.57735;
var x3 >= -0.57735 <= 0.57735;
var x4 >= -0.57735 <= 0.57735;
var x5 >= -0.57735 <= 0.57735;
var x6 >= -0.57735 <= 0.57735;
var x7 >= -0.57735 <= 0.57735;
var x8 >= -0.57735 <= 0.57735;
var x9 >= -0.57735 <= 0.57735;
var x10 >= -0.57735 <= 0.57735;
var x11 >= -0.57735 <= 0.57735;
var x12 >= -0.57735 <= 0.57735;
var x13 >= -0.57735 <= 0.57735;
var x14 >= -0.57735 <= 0.57735;
var x15 >= -0.57735 <= 0.57735;
var x16 >= -0.57735 <= 0.57735;
var x17 >= -0.57735 <= 0.57735;
minimize dummy_obj: 0;
subject to eqn0: -0.333333+x13*x13+x12*x12 = 0;
subject to eqn1: 0.333333+x6*x8+x0*x2+x1*x3+x7*x9+-1.000000*x12 = 0;
subject to eqn2: x0*x8+x1*x9+-1.000000*x13+-1.000000*x2*x6+-1.000000*x3*x7 = 0;
subject to eqn3: -0.333333+x15*x15+x14*x14 = 0;
subject to eqn4: x0*x4+-1.000000*x14+x7*x11+0.333333+x6*x10+x1*x5 = 0;
subject to eqn5: x1*x11+x0*x10+-1.000000*x4*x6+-1.000000*x5*x7+-1.000000*x15 = 0;
subject to eqn6: x6*x6+-0.333333+x0*x0 = 0;
subject to eqn7: -0.333333+x7*x7+x1*x1 = 0;
subject to eqn8: 0.333333*x1*x1+0.333333*x7*x7+0.666667*x0*x1+0.384900*x0+0.384900*x1+0.666667*x6*x7+-0.222222+0.333333*x0*x0+0.333333*x6*x6 = 0;
subject to eqn9: -0.333333+x17*x17+x16*x16 = 0;
subject to eqn10: 0.333333+x8*x10+x3*x5+x9*x11+x2*x4+-1.000000*x16 = 0;
subject to eqn11: -1.000000*x5*x9+-1.000000*x17+-1.000000*x4*x8+x2*x10+x3*x11 = 0;
subject to eqn12: -0.333333+x8*x8+x2*x2 = 0;
subject to eqn13: -0.333333+x9*x9+x3*x3 = 0;
subject to eqn14: 0.333333*x3*x3+0.666667*x2*x3+0.384900*x2+-0.222222+0.333333*x2*x2+0.384900*x3+0.333333*x9*x9+0.666667*x8*x9+0.333333*x8*x8 = 0;
subject to eqn15: -0.333333+x10*x10+x4*x4 = 0;
subject to eqn16: -0.333333+x11*x11+x5*x5 = 0;
subject to eqn17: 0.666667*x10*x11+0.333333*x10*x10+0.333333*x4*x4+0.333333*x5*x5+-0.222222+0.384900*x4+0.666667*x4*x5+0.333333*x11*x11+0.384900*x5 = 0;
subject to pos0: -4.000000*x8+-1.000000*x9+-8.000000*x2+-2.000000*x3+8.000000*x0+4.000000*x6+2.000000*x1+x7 >= 0;
subject to pos1: 4.000000*x8+-2.000000*x5+-4.000000*x10+2.000000*x3+-1.000000*x11+8.000000*x2+-8.000000*x4+x9 >= 0;
subject to pos2: 32.000000*x0+-32.000000*x1+16.000000*x6+4.000000*x8+-16.000000*x7+-2.000000*x5+-1.000000*x11+-8.000000*x3+x10+-4.000000*x9+2.000000*x4+8.000000*x2 >= 0;
subject to pos3: x11 >= 0;
solve;
