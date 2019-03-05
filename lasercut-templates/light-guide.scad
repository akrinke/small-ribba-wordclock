// width of the laser cut
kerf = 0.0;
// minimum distance between parts on the panel
min_distance = 1+kerf;

// light guide
thickness = 1;
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
    blockages();
    horizontal_light_guide();
    vertical_light_guide();
} else {
    flat();
}

// used to cut out the notches of the individual light guide pieces
module hnotches() {
    color("red", 0.5)
        translate([0,0,-height/2])
            horizontal_light_guide();
}
module vnotches() {
    color("red", 0.5)
        translate([0,0,height/2])
            vertical_light_guide();
}

// horizontal stripes
// ==================

// outer ring bottom
module h0() {
    rotate([-90,0,0])
        translate([thickness,thickness,pcb_height])
            difference() {
                translate([-thickness,-thickness,-pcb_height])
                    cube([200+2*thickness,thickness,height+pcb_height]);
                vnotches();
            }
}    

// 7805 (between row 0 (outer ring) and row 1
module h0_7805() {
    rotate([-90,0,0])
        translate([-172+overlap,-10,0])
            difference() {
                translate([172-overlap,10-thickness/2])
                    cube([14+overlap+thickness/2,thickness,height]);
                vnotches();
            }
}

// rows 1, 2, 8
module h1_2_8() {
    rotate([-90,0,0])
        translate([overlap,-28])
            difference() {
                translate([-overlap,28-thickness/2])
                    cube([186+overlap+thickness/2,thickness,height]);
                vnotches();
            }
}

// rows 3, 7, 9
module h3_7_9() {
    rotate([-90,0,0])
        translate([overlap,-64])
            difference() {
                translate([-overlap,64-thickness/2])
                    cube([200+2*overlap,thickness,height]);
                vnotches();
            }   
}

// row 4
module h4() {
    rotate([-90,0,0])
        translate([overlap,-82])
            difference() {
                translate([-overlap,82-thickness/2])
                    cube([200+2*overlap,thickness,height]);
                vnotches();
            }       
}

// row 5
module h5() {
    rotate([-90,0,0])
        translate([overlap,-100])
            difference() {
                translate([-overlap,100-thickness/2])
                    cube([200+2*overlap,thickness,height]);
                vnotches();
                blockages();
            }    
}

// row 6
module h6() {
    rotate([-90,0,0])
        translate([overlap,-118])
            difference() {
                translate([-overlap,118-thickness/2])
                    cube([172+overlap+thickness/2,thickness,height]);
                vnotches();
            }
}

// outer ring top
module h10() {
    rotate([-90,0,0])
        translate([thickness,-200,pcb_height])
            difference() {
                translate([-thickness,200,-pcb_height])
                    cube([200+2*thickness,thickness,height+pcb_height]);
                vnotches();
            }
}

// vertical stripes
// ================

// outer ring left
module v0() {
    rotate([0,-90,0])
        translate([thickness,thickness,-height])
            difference() {
                translate([-thickness,-thickness,-pcb_height])
                    cube([thickness,200+2*thickness,height+pcb_height]);
                hnotches();
            }
        
}

// default vertical stripe (9x; column 1-9)
module v1_9() {
    rotate([0,-90,0])
        translate([-28,overlap,-height])
            difference() {
                translate([28-thickness/2,-overlap])
                    cube([thickness,200+2*overlap,height]);
                hnotches();
            }
}

// column 10
module v10() {
    rotate([0,-90,0])
        translate([-172,overlap,-height])
            difference() {
                translate([172-thickness/2,-overlap])
                    cube([thickness,200+2*overlap,height]);
                hnotches();
            }
}

// column 11
module v11_7805() {
    rotate([0,-90,0])
        translate([-186,-10+overlap,-height])
            difference() {
                translate([186-thickness/2,10-overlap])
                    cube([thickness,18+2*overlap,height]);
                hnotches();
            }
}

module v11_capacitors() {
    rotate([0,-90,0])
        translate([-186,-46+thickness/2,-height])
            difference() {
                translate([186-thickness/2,46-thickness/2])
                    cube([thickness,18+overlap,height]); 
                hnotches();
            }
}

module v11_isp_port() {
    rotate([0,-90,0])
        translate([-188,-172+2*18+overlap,-height])
            difference() {
                translate([188-thickness/2,172-2*18-overlap])
                    cube([thickness,2*18+2*overlap,height]);
                hnotches();
            }
}

// outer ring right
module v12() {
    rotate([0,-90,0])
        translate([-200,thickness,-height])
            difference() {
                translate([200,-thickness,-pcb_height])
                cube([thickness,200+2*thickness,height+pcb_height]);
                hnotches();
            }
}

// diagonal corner pieces
module corner() {
    rotate([-90,0,0])
        translate([0,7])
            rotate([0,0,45])
                translate([0,-14-2*overlap,0])
                    difference() {
                        translate([7,7,height/2])
                            rotate([0,0,45])
                                cube([thickness,(14+2*overlap)*sqrt(2),height],center=true);
                        // enlarge gap width in diagonal corner piece
                        translate([14,-thickness/2,0])
                            cube([10,(1+2*cos(45))*thickness,height],center=true);
                        translate([-thickness/2,14,height])
                            cube([(1+2*cos(45))*thickness,10,height],center=true);
                    }
}

module flat() {
    //translate([0,-500])
    offset(delta=kerf/2) {
        projection() {
            h0();
            translate([0,pcb_height]) {
                translate([180,6*(height+min_distance)])
                    h0_7805();
                translate([0,1*(height+min_distance)])
                    h1_2_8();
                translate([0,2*(height+min_distance)])
                    h1_2_8();
                translate([0,3*(height+min_distance)])
                    h3_7_9();
                translate([0,4*(height+min_distance)])
                    h4();
                translate([0,5*(height+min_distance)])
                    h5();
                translate([0,6*(height+min_distance)])
                    h6();
                translate([0,7*(height+min_distance)])
                    h3_7_9();
                translate([0,8*(height+min_distance)])
                    h1_2_8();
                translate([0,9*(height+min_distance)])
                    h3_7_9();
                translate([0,10*(height+min_distance)])
                    h10();
                translate([0,11*(height+min_distance)+pcb_height]) {
                    corner();
                    translate([40,0])
                        corner();
                    translate([80,0])
                        corner();
                    translate([120,0])
                        corner();
                }
            }
            translate([0,12*(height+min_distance)+2*pcb_height]) {
                v0();
                translate([pcb_height,0]) {
                    translate([1*(height+min_distance),0])
                        v1_9();
                    translate([2*(height+min_distance),0])
                        v1_9();
                    translate([3*(height+min_distance),0])
                        v1_9();
                    translate([4*(height+min_distance),0])
                        v1_9();
                    translate([5*(height+min_distance),0])
                        v1_9();
                    translate([6*(height+min_distance),0])
                        v1_9();
                    translate([7*(height+min_distance),0])
                        v1_9();
                    translate([8*(height+min_distance),0])
                        v1_9();
                    translate([9*(height+min_distance),0])
                        v1_9();
                    translate([10*(height+min_distance),0])
                        v10();
                    translate([11*(height+min_distance),0]) {
                        v11_7805();
                        translate([0,30])
                            v11_capacitors();
                        translate([0,60])
                            v11_isp_port();
                    }
                    translate([12*(height+min_distance),0])
                        v12();
                }
            }
        }
    }
}

module horizontal_light_guide() {
    // outer ring
    translate([-thickness,-thickness,-pcb_height])
        cube([200+2*thickness,thickness,height+pcb_height]);
    translate([-thickness,200,-pcb_height])
        cube([200+2*thickness,thickness,height+pcb_height]);


    // inner stripes
    difference() {
        for (y=[28:18:172])
            translate([-overlap,y-thickness/2])
                cube([200+2*overlap,thickness,height]);
        translate([186+thickness/2,25,-1])
            cube([30,30,20]);
        translate([172+thickness/2,112,-1])
            cube([40,10,20]);
    }

    // 7805
    translate([172-overlap,10-thickness/2])
        cube([14+overlap+thickness/2,thickness,height]);

    // corners
    difference() {
        translate([7,7,height/2])
            rotate([0,0,45])
                // (1+1/cos(45))*thickness is the required gap width for interlocking with a 45 degree rotated piece with the same thickness.
                // However, after projection, the real gap width when subtracting a piece of width t2 is thictkness+t2/cos(45)-2*thickness
                // Therefore, a width of (1+2*cos(45))*thickness is required.
                cube([(1+2*cos(45))*thickness,(14+2*overlap)*sqrt(2),height],center=true);
        translate([4,-10,-1])
            cube([20,20,2*height]);
    }
    
    difference() {
        translate([7,200-7,height/2])
            rotate([0,0,-45])
                cube([(1+2*cos(45))*thickness,(14+2*overlap)*sqrt(2),height],center=true);
        translate([4,200-10,-1])
            cube([20,20,2*height]);
    }

    difference() {
        translate([200-7,200-7,height/2])
            rotate([0,0,45])
                cube([(1+2*cos(45))*thickness,(14+2*overlap)*sqrt(2),height],center=true);
        translate([200-20-4,200-10,-1])
            cube([20,20,2*height]);
    }

    difference() {
        translate([200-7,7,height/2])
            rotate([0,0,-45])
                cube([(1+2*cos(45))*thickness,(14+2*overlap)*sqrt(2),height],center=true);
        translate([200-20-4,-10,-1])
            cube([20,20,2*height]);
    }
}

module vertical_light_guide() {
    // outer ring
    translate([-thickness,-thickness,-pcb_height])
        cube([thickness,200+2*thickness,height+pcb_height]);    
    translate([200,-thickness,-pcb_height])
        cube([thickness,200+2*thickness,height+pcb_height]);
    
    // inner stripes
    for (x=[28:16:172])
        translate([x-thickness/2,-overlap])
            cube([thickness,200+2*overlap,height]);

    // 7805
    translate([186-thickness/2,10-overlap])
        cube([thickness,18+2*overlap,height]);
    // capacitors
    translate([186-thickness/2,46-thickness/2])
        cube([thickness,18+overlap,height]);    
    // ISP port
    translate([188-thickness/2,172-2*18-overlap])
        cube([thickness,2*18+2*overlap,height]);
    
    // corners
    difference() {
        translate([7,7,height/2])
            rotate([0,0,45])
                cube([(1+2*cos(45))*thickness,(14+2*overlap)*sqrt(2),height],center=true);
        translate([-20+4,4,-1])
            cube([20,20,2*height]);
    }
    
    difference() {
        translate([7,200-7,height/2])
            rotate([0,0,-45])
                cube([(1+2*cos(45))*thickness,(14+2*overlap)*sqrt(2),height],center=true);
        translate([-20+4,200-20,-1])
            cube([20,20,2*height]);
    }
        
    difference() {
        translate([200-7,200-7,height/2])
            rotate([0,0,45])
                cube([(1+2*cos(45))*thickness,(14+2*overlap)*sqrt(2),height],center=true);
        translate([200-4,200-20,-1])
            cube([20,20,2*height]);
    }
    
    difference() {
        translate([200-7,7,height/2])
            rotate([0,0,-45])
                cube([(1+2*cos(45))*thickness,(14+2*overlap)*sqrt(2),height],center=true);
        translate([200-4,0,-1])
            cube([20,20,2*height]);
    }
}

module blockages() {
    color("Blue") {
        translate([188+thickness/2,172-2*18+thickness/2,-5])
            cube([12+overlap,2*18-thickness,25]);
        translate([186+thickness/2,25,-1])
            cube([14-thickness/2,30,20]);
        translate([172+thickness/2,112,-1])
            cube([28-thickness/2,10,20]);
        // battery holder
        translate([182,200-104,-1])
            cube([16,22,5.5+1+0.2]);
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