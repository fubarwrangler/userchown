General User Chown & Copy
=========================

A program that allows the a specified user to copy files as any other user
belonging to a specified group to a specific set of directories.

Design
------
A setuid binary is required, so we may as well write in C as it isn't that
large a program. Will be packaged as an RPM and installed on PHENIX nodes.

Reads a configuration file to determine the allowed user, target-user's group,
and target locations.

Limitations
-----------
The program will become the target user to copy files, so the inputs must be
readable by that user (likely world-readable). The target directories must be
enumerated in the config file and must be underneath this. They also cannot
contain any back-references "../" or symbolic links that point outside of
this target directory.
