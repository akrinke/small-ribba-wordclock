// width of the laser cut
kerf = 0.05;
// minimum distance between parts on the panel
min_distance = 1+kerf;

// light guide
thickness = 1.0;
height = 16;
overlap = 5;

// pcb
pcb_height = 1.6;

render_3d = false;

if (render_3d) {
    pcb();
    components();
    leds();
    screws();
    //blockages();
    hstripes();
    vstripes();
    corners();
} else {
    flat();
}

// horizontal stripes
// ==================

horiz_cubes = [
    // h0
    [200+2*overlap,thickness,height+pcb_height],
    // h0_7805
    [14+overlap+thickness/2,thickness,height],
    // h1-h10
    [186+overlap+thickness/2,thickness,height],
    [186+overlap+thickness/2,thickness,height],
    [200+2*overlap,thickness,height],
    [200+2*overlap,thickness,height],
    [200+2*overlap,thickness,height],
    [172+overlap+thickness/2,thickness,height],
    [200+2*overlap,thickness,height],
    [188+overlap+thickness/2,thickness,height],
    [200+2*overlap,thickness,height],
    [200+2*overlap,thickness,height+pcb_height]
];

// translation of horizontal stripes to their correct 3d position
horiz_trans = concat([
    // h0
    [-overlap,-thickness,-pcb_height],
    // h0_7805
    [172-overlap,10-thickness/2]],
    // h1-h9
    [for (y=[28:18:182]) [-overlap, y-thickness/2]],
    // h10
    [[-overlap,200,-pcb_height]]
);

// translation of horizontal stripes to their position on the flat sheet
horiz_flat_trans = concat([
    // h0
    [0,0],
    // h0_7805
    [180,6*(height+min_distance)+pcb_height]],
    // h1-h10
    [for (i=[1:10]) [0,i*(height+min_distance)+pcb_height]]
);


// vertical stripes
// ================

vert_cubes = concat([
    // v0
    [thickness,200+2*overlap,height+pcb_height],
    // v0_PWM
    [thickness,2*18+2*overlap,height]],
    // v1-v10
    [for (x=[28:16:172]) [thickness,200+2*overlap,height]],[
    // v11_7805
    [thickness,18+2*overlap,height],
    // v11_capacitors
    [thickness,18+overlap,height],
    // v11_isp_port
    [thickness,2*18+2*overlap,height],
    // v12
    [thickness,200+2*overlap,height+pcb_height]]
);

// translation of vertical stripes to their correct 3d position
vert_trans = concat([
    // v0
    [-thickness,-overlap,-pcb_height],
    // v0_PWM
    [12-thickness/2,82-overlap]],
    // v1-v10
    [for (x=[28:16:172]) [x-thickness/2,-overlap]],[
    // v11_7805
    [186-thickness/2,10-overlap],
    // v11_capacitors
    [186-thickness/2,46-thickness/2],
    // v11_isp_port
    [188-thickness/2,172-2*18-overlap],
    // v12
    [200,-overlap,-pcb_height]]
);

// translation of vertical stripes to their position on the flat sheet
vert_flat_trans = concat([
    // v0
    [0,12*(height+min_distance)+2*pcb_height],
    // v0_PWM
    [11*(height+min_distance)+pcb_height,12*(height+min_distance)+2*pcb_height+110]],
    // v1-v10, v11_7805
    [for (i=[1:11]) [i*(height+min_distance)+pcb_height,12*(height+min_distance)+2*pcb_height]],[
    // v11_capacitors
    [11*(height+min_distance)+pcb_height,12*(height+min_distance)+2*pcb_height+30],
    // v11_isp_port
    [11*(height+min_distance)+pcb_height,12*(height+min_distance)+2*pcb_height+60],
    // v12
    [12*(height+min_distance)+pcb_height,12*(height+min_distance)+2*pcb_height]]
);

// corner segments
// ===============

// length of a diagonal corner segment
corner_length = (14+2*overlap)*sqrt(2);

/*
    (1+1/cos(45))*thickness is the required gap width for interlocking with a 45 degree rotated piece with the same thickness.
    However, after projection, the real gap width when subtracting a piece of width t2 is thickness+t2/cos(45)-2*thickness.
    Therefore, a width of (1+2*cos(45))*thickness is required.
*/
corner_gap_width = (1+2*cos(45))*thickness;


module corner_cube() {
    cube([thickness,corner_length,height],center=true);
}

module corner_notch_cube() {
    cube([corner_gap_width,corner_length,height],center=true);
}

corner_trans = [
    [7,7,height/2],
    [7,200-7,height/2],
    [200-7,200-7,height/2],
    [200-7,7,height/2]
];

corner_flat_trans = [for (x=[0:40:120]) [x,11*(height+min_distance)+2*pcb_height]];

corner_rot = [
    [0,0,45],
    [0,0,-45],
    [0,0,45],
    [0,0,-45]
];

// used to cut out the notches of the individual light guide pieces
module hnotches() {
    color("red", 0.5)
        translate([0,0,-height/2]) {
            for (i=[0:len(horiz_trans)-1])
                translate(horiz_trans[i])
                    cube(horiz_cubes[i]);
            // corner segments
            difference() {
                for (i=[0:3])
                    translate(corner_trans[i])
                        rotate(corner_rot[i])
                            corner_notch_cube();
                // use only one half of the corner segment
                for (x=[4,200-20-4],y=[-10,200-10])
                    translate([x,y,-1])
                        cube([20,20,2*height]);
            }
        }
}

module vnotches() {
    color("red", 0.5)
        translate([0,0,height/2]) {
            for (i=[0:len(vert_trans)-1])
                translate(vert_trans[i])
                    cube(vert_cubes[i]);
            // corner segments
            difference() {
                for (i=[0:3])
                    translate(corner_trans[i])
                        rotate(corner_rot[i])
                            corner_notch_cube();
                // use only one half of the corner segment
                for (x=[-20+4,200-4],y=[4,200-20])
                    translate([x,y,-1])
                        cube([20,20,2*height]);
            }
        }
}

module corner_notches() {
    translate([0,0,-height/2])
        for (x=[14,200-14],y=[-thickness/2,200+thickness/2])
            translate([x,y,height/2])
                cube([10,corner_gap_width,height],center=true);
    translate([0,0,height/2])
        for (x=[-thickness/2,200+thickness/2],y=[14,200-14])
            translate([x,y,height/2])
                cube([corner_gap_width,10,height],center=true);
}

module hstripe(i) {
    difference() {
        translate(horiz_trans[i])
            cube(horiz_cubes[i]);
        vnotches();
        blockages();
    }
}

module hstripes() {
    for (i=[0:len(horiz_trans)-1])
        hstripe(i);
}

module vstripe(i) {
    difference() {
        translate(vert_trans[i])
            cube(vert_cubes[i]);
        hnotches();
        blockages();
    }
}

module vstripes() {
    for (i=[0:len(vert_trans)-1])
        vstripe(i);
}

module corner(i) {
    difference() {
        translate(corner_trans[i])
            rotate(corner_rot[i])
                corner_cube();
        corner_notches();
        blockages();
    }
}

module corners() {
    for (i=[0:3])
        corner(i);
}

module flat() {
    offset(delta=kerf/2) {
        projection() {
            // horizontal segments
            for (i=[0:len(horiz_trans)-1])
                translate(horiz_flat_trans[i])
                    rotate([-90,0,0])
                        translate(-horiz_trans[i])
                            hstripe(i);
            // corners
            for (i=[0:3])
                translate(corner_flat_trans[i])
                    translate([corner_length/2,height/2])
                        rotate([-90,0,0])
                            rotate(corner_rot[i])
                                translate(-corner_trans[i])
                                    corner(i);

            // vertical segments
            for (i=[0:len(vert_trans)-1])
                translate(vert_flat_trans[i])
                    rotate([0,90,0])
                        translate(-vert_trans[i])
                            vstripe(i);
        }
    }
}

module blockages() {
    color("Blue") {
        // ISP port
        translate([200-10.5,137])
            cube([14,20.5,9]);
        // battery holder
        translate([182,200-104,-1])
            cube([16,22,5.5+1+0.2]);
        // IR sensor
        translate([156+thickness/2,200-64-thickness])
            cube([16-thickness,2*thickness,height+1]);
        // PWM transistor IRLU2905
        translate([4.5,95.6])
            cube([6.9,12,2.5]);
    }
}

module pcb() {
    $fn = 30; // number of fragments
    color("Green")
        translate([0,0,-pcb_height])
            difference() {
                cube([200,200,pcb_height]);
                for (x=[10,100,190])
                    for (y=[10,190])
                        translate([x,y])
                            cylinder(h=3*pcb_height, d=3.3, center=true);
            }
}

module components() {
    $fn = 30; // number of fragments
    color("Turquoise") {
        // 7805
        translate([187,200-187])
            cube([11,22,4.43]);
        // PWM transistor IRLU2905
        translate([4.5,95.6])
            cube([6.9,12,2.5])
        // capacitors
        translate([187.833,200-159.766])
            cylinder(h=12.3, d=10.3);
        translate([194.818,200-150.876])
            cylinder(h=12.3, d=10.3);
        translate([164,200-73.08])
            cylinder(h=12.3, d=10.3);
        // battery holder
        translate([182,200-104])
            cube([16,22,5.5+0.2]);
        // ISP port
        translate([200-10.5,137])
            cube([14,20.5,8.91]);
    }
}

module leds() {
    height = 5;
    diameter = 5;
    $fn = 30; // number of fragments

    color("Red") {
        for (x=[20 : 16 : 180])
            for (y=[19 : 18 : 181])
                translate([x,y])
                    cylinder(height, d=diameter);
        for (x=[4,196])
            for (y=[4,196])
                translate([x,y])
                    cylinder(height, d=diameter);
    }
}

module screws() {
    $fn = 30; // number of fragments

    color("Blue")
        for (x=[10,100,190])
            for (y=[10,190])
                translate([x, y]) {
                    // washer
                    cylinder(0.5, d=6.75);
                    // hex nut
                    cylinder(2.32, d=6.1, $fn=6);
                }
}
