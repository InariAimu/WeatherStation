
thickness=2.5;

difference()
{
    translate([-4,-4,0])
        cube(size=[70+4*2,50+4*2,3]);
    translate([0+2,0+2,2])
        cube(size=[70-4,50-4,1]);
}


translate([0+2,0+2,0])
    cylinder(d=2,h=6,$fn=12);
translate([70-2,0+2,0])
    cylinder(d=2,h=6,$fn=12);
translate([0+2,50-2,0])
    cylinder(d=2,h=6,$fn=12);
translate([70-2,50-2,0])
    cylinder(d=2,h=6,$fn=12);

difference()
{

    difference()
    {
        translate([-4,-4,3])
            cube(size=[70+4*2,50+4*2,20]);
        translate([-thickness,-thickness,3])
            cube(size=[70+thickness*2,50+thickness*2,20]);
    }

    
    union()
    {
        translate([-4,25,5])
            cube([3,20,8]);
        translate([-4,5,3+16])
            cube([3,17,4]);
        
        translate([39,-4,9])
            cube([16,4,6]);
        
        translate([70,14,5])
            cube([4,34,6]);
        
        
        translate([10,-3.5,18])
            cube([5,1,3]);
        translate([70-10-5,-3.5,18])
            cube([5,1,3]);
        translate([10,50+2.5,18])
            cube([5,1,3]);
        translate([70-10-5,50+2.5,18])
            cube([5,1,3]);
    }
}


translate([-4,25+14,5])
    cube([1.5,0.5,8]);
translate([-4,25+6,5])
    cube([1.5,0.5,8]);

translate([70+2.5,14+5,5])
    cube([1.5,0.5,6]);
translate([70+2.5,14+10,5])
    cube([1.5,0.5,6]);
translate([70+2.5,14+15,5])
    cube([1.5,0.5,6]);
translate([70+2.5,14+20,5])
    cube([1.5,0.5,6]);
translate([70+2.5,14+25,5])
    cube([1.5,0.5,6]);
translate([70+2.5,14+30,5])
    cube([1.5,0.5,6]);

translate([39+4,-4,9])
    cube([0.5,1.5,6]);
translate([39+8,-4,9])
    cube([0.5,1.5,6]);
translate([39+12,-4,9])
    cube([0.5,1.5,6]);

difference()
{
    translate([70-18+4,-4-18,0])
        cube(size=[18,20,2]);

    translate([70-18+4+9,-4-18+9,0])
        cylinder(d=3.2,h=3,$fn=24);
}


translate([-4,-4-34-2,0])
    cube([5,34+2,2]);
translate([-4,-4-2,4])
    cube([5,2,2]);
translate([-4,-4-34-2,2])
    cube([5,2,4]);
translate([-4,-4-34,4])
    cube([5,2,2]);


translate([-4+46-5,-4-34,0])
    cube([5,34,2]);
translate([-4+46-5,-4-2,4])
    cube([5,2,2]);
translate([-4+46-5,-4-34,2])
    cube([5,2,6]);


translate([-4,50+4,0])
    cube([10,20,2]);
translate([-4+70-2,50+4,0])
    cube([10,20,2]);

difference()
{
    translate([-4+38-5,50+4,0])
        cube([10,20,2]);
    
    translate([-4+38,50+4+11,0])
        cylinder(d=3.2,h=3,$fn=12);
}
