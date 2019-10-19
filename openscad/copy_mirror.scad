// Borrowed from http://forum.openscad.org/Wish-mirror-copy-true-td10681.html
module copy_mirror(vec=[0,1,0])
{
    children();
    mirror(vec) children();
}
