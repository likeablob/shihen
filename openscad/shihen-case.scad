$fn=30;

include <copy_mirror.scad>

BOARD_X = 83+2*2;
BOARD_Y = 27.5+2*2;

DISP_X=89;
DISP_Y=36;
DISP_Z=1.5;

DISP_E_X=68.3;
DISP_E_Y=30.5;

CASE_X=DISP_X+2;
CASE_Y=DISP_Y+2;
CASE_Z=DISP_Z+6;

SIDE_B_EDGE_Y=3.5*2;
SIDE_B_YPOS=4;

BOARD_ANGLE=80;

module display() {
  translate([0, 0, DISP_Z/2])
  cube(size=[DISP_X, DISP_Y, DISP_Z], center=true);

  translate([-DISP_X/2+DISP_E_X/2+7.2, 0, DISP_Z])
  color("green")
  cube(size=[DISP_E_X, DISP_E_Y, 0.1], center=true);
}

DISP_ANGLE=70;
*%translate([0, -DISP_Y*cos(DISP_ANGLE)/2-1, DISP_Y*sin(DISP_ANGLE)/2])
rotate([DISP_ANGLE, 0, 0])
display();

BAT_X=43;
BAT_Y=21.7;
BAT_Z=5.7;
// BAT
*%color("red")
translate([0, -(DISP_Y)*cos(DISP_ANGLE)/2+3, BAT_Y*sin(DISP_ANGLE)/2+6])
rotate([DISP_ANGLE, 0, 0])
cube(size=[BAT_X, BAT_Y, BAT_Z], center=true);

module caseBase(args) {
  hull() {
    translate([0, 0, DISP_Y*sin(DISP_ANGLE)])
    rotate([0, 90, 0])
    cylinder(d=0.01, h=CASE_X, center=true);

    translate([0, 1, DISP_Y*sin(DISP_ANGLE)])
    rotate([0, 90, 0])
    cylinder(d=0.01, h=CASE_X, center=true);

    translate([0, -DISP_Y*cos(DISP_ANGLE), 0])
    rotate([0, 90, 0])
    cylinder(d=0.01, h=CASE_X, center=true);

    translate([0, BOARD_Y*cos(BOARD_ANGLE)+SIDE_B_YPOS, 0])
    rotate([0, 90, 0])
    cylinder(d=0.01, h=CASE_X, center=true);
  }
}

module case() {
  difference() {
    union() {
      difference() {
        caseBase();

        // inner space
        translate([0, -1.0, 1])
        translate([0, 0, 0])
        resize([CASE_X-SIDE_B_EDGE_Y, 0, 0])
        caseBase();

        // display con
        CON_A_Z=3;
        translate([0, -DISP_Y*cos(DISP_ANGLE)/2-1, DISP_Y*sin(DISP_ANGLE)/2])
        rotate([DISP_ANGLE, 0, 0])
        translate([-(DISP_X/2-10/2)-1, 0, -CON_A_Z/2])
        hull() {
          cube(size=[10, 18.5, CON_A_Z], center=true);
          translate([0, 0, -CON_A_Z/2-1.5])
          rotate([0, 90, 0])
          cylinder(d=0.01, h=10, center=true);
        }

        // screw holes
        translate([0, -DISP_Y*cos(DISP_ANGLE)/2-1, DISP_Y*sin(DISP_ANGLE)/2])
        rotate([DISP_ANGLE, 0, 0])
        copy_mirror([1,0,0])
        copy_mirror([0,1,0])
        difference() {
          translate([DISP_X/2-2.25, -DISP_Y/2+2.25, -5])
          cylinder(d=1.5, h=CASE_Z, center=!true);
        }

        // push sw
        translate([0, -DISP_Y*cos(DISP_ANGLE)/2-1, DISP_Y*sin(DISP_ANGLE)/2])
        rotate([DISP_ANGLE, 0, 0])
        translate([-DISP_X/2+47.75, DISP_Y/2-5.25, 0])
        cube(size=[7, 7, CASE_Z*2], center=true);

        // sd slot
        translate([0, -DISP_Y*cos(DISP_ANGLE)/2-1, DISP_Y*sin(DISP_ANGLE)/2])
        rotate([DISP_ANGLE, 0, 0])
        translate([-DISP_X/2+25.5, DISP_Y/2, 0])
        cube(size=[16, 20, 8], center=true);
      }

      // magnet
      intersection() {
        resize([CASE_X-SIDE_B_EDGE_Y, 0, 0])
        caseBase();

        copy_mirror([1, 0, 0])
        translate([BOARD_X/2-10, 0, 0])
        hull() {
          cylinder(d=10+3, h=6, center=false);
          translate([0, (10+3), 6/2])
          cube(size=[10+3, 0.01, 6], center=true);
        }
      }

      // frame
  *    difference() {
        resize([CASE_X-SIDE_B_EDGE_Y, 0, 0])
        caseBase();
        translate([0, 0, BOARD_Y*sin(BOARD_ANGLE)/2+1])
        rotate([BOARD_ANGLE, 0, 0])
        resize([BOARD_X*2, BOARD_Y, 0])
        cylinder(d=1, h=40, center=true);
      }
    } // union

    // magnet
    copy_mirror([1, 0, 0])
    translate([BOARD_X/2-10, 0, -0.01])
    union() {
      cylinder(d=10+1, h=2.7, center=false);
      cylinder(d=1.5, h=6, center=false);
    }

    // edge
    translate([0, -DISP_Y*cos(DISP_ANGLE)/2, DISP_Y*sin(DISP_ANGLE)/2])
    rotate([DISP_ANGLE, 0, 0])
    translate([0, 0, -1/2])
    cube(size=[DISP_X-SIDE_B_EDGE_Y, DISP_Y+20, 2], center=true);

    // USB
    translate([0, -1, BOARD_Y*sin(BOARD_ANGLE)/2])
    rotate([DISP_ANGLE, 0, 0]) {

      translate([BOARD_X/2-5, -BOARD_Y/2+15.5, 4.5+1])
      rotate([0, 180, -90])
      USB_BOARD_DIF();

  *    translate([CASE_X/2-1, -BOARD_Y/2+14.8, 2])
      rotate([0, 90, 0])
      hull() {
        copy_mirror([0, 1, 0])
        translate([0, 7/2, 0])
        cylinder(d=7, h=10, center=!true);
      }
    }
  }
}

case();


USB_X = 18;
USB_Y = 15;
module USB_BOARD_DIF(scZ) {

  // BOARD
  difference() {
    translate([0, 0, 1.5/2])
    cube(size=[USB_X, USB_Y, 1.5], center=true);

    copy_mirror([1,0,0])
    translate([USB_X/2-3.3/2-1, -USB_Y/2+3.3/2+4, 0])
    cylinder(d=3.3, h=10, center=true);
  }

  // PORT
  USB_P_Y_OFFSET = 10;
  USB_P_Y = 8+USB_P_Y_OFFSET;
  translate([0, USB_Y/2-USB_P_Y/2 + USB_P_Y_OFFSET, 1.5-0.3]){
    hull() {
      // top
      translate([0, 0, 3.2])
      cube(size=[6.2+0.2, USB_P_Y, 0.01], center=true);

      // middle
      translate([0, 0, 1.55])
      cube(size=[8.0+0.2, USB_P_Y, 0.5], center=true);

      // bottom
      translate([0, 0, 0])
      cube(size=[5.2+1, USB_P_Y, 0.01], center=true);
    }
  }

  // screws
  copy_mirror([1,0,0])
  translate([USB_X/2-3.3/2-1, -USB_Y/2+3.3/2+4, -scZ/2])
  cylinder(d=2.2, h=scZ, center=true);
}
