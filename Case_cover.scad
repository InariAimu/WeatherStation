
//15
//2
//22

tl = 2.3;

difference()
{
    union()
    {
        translate([-4,-4,0])
            cube(size=[70+4*2,50+4*2,1]);
        
        translate([-tl,-tl,0])
            cube(size=[70+2*tl,50+2*tl,3]);
    }
    
    union()
    {
        translate([-tl+1,-tl+1,.24+.2*3])
            cube(size=[70+2*(tl-1),50+2*(tl-1),3]);
        
        translate([74-15,5,0])
            cube([15,17,4]);
        
        translate([16-4-1.5,9-4-1.5,0])
            cube([3,3,3]);
        
        translate([74-44-9,18-3-4,0])
            cube([3*3,3,3]);
    }
}

translate([10,-tl,3])
    cube([5,1,3]);
translate([70-10-5,-tl,3])
    cube([5,1,3]);
translate([10,50+tl-1,3])
    cube([5,1,3]);
translate([70-10-5,50+tl-1,3])
    cube([5,1,3]);

translate([10,-tl,4])
    rotate([45,0,0])
        cube([5,1,1]);
translate([70-10-5,-tl,4])
    rotate([45,0,0])
        cube([5,1,1]);
translate([10,50+tl-1+1,4])
    rotate([45,0,0])
        cube([5,1,1]);
translate([70-10-5,50+tl-1+1,4])
    rotate([45,0,0])
        cube([5,1,1]);

