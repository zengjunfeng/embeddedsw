###############################################################################
#
# Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# Use of the Software is limited solely to applications:
# (a) running on a Xilinx device, or
# (b) that interact with a Xilinx device through a bus or interconnect.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# XILINX CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
# OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# Except as contained in this notice, the name of the Xilinx shall not be used
# in advertising or otherwise to promote the sale, use or other dealings in
# this Software without prior written authorization from Xilinx.
#
###############################################################################
# MODIFICATION HISTORY:
# Ver      Who    Date     Changes
# -------- ------ -------- ------------------------------------
# 5.0      adk    12/10/13 Updated as per the New Tcl API's

proc generate {drv_handle} {
    xdefine_include_file $drv_handle "xparameters.h" "XSCALER" "NUM_INSTANCES" "DEVICE_ID" "C_BASEADDR" "C_HIGHADDR" "C_NUM_V_TAPS" "C_NUM_H_TAPS" "C_MAX_PHASES" "C_MAX_COEF_SETS" "C_CHROMA_FORMAT" "C_SEPARATE_YC_COEFS" "C_SEPARATE_HV_COEFS"
    xdefine_config_file $drv_handle "xscaler_g.c" "XScaler" "DEVICE_ID" "C_BASEADDR" "NUM_V_TAPS" "NUM_H_TAPS" "MAX_PHASES" "MAX_COEF_SETS" "C_CHROMA_FORMAT" "C_SEPARATE_YC_COEFS" "C_SEPARATE_HV_COEFS"
    xdefine_canonical_xpars $drv_handle "xparameters.h" "XScaler" "DEVICE_ID" "C_BASEADDR" "C_HIGHADDR" "C_NUM_V_TAPS" "C_NUM_H_TAPS" "C_MAX_PHASES" "C_MAX_COEF_SETS" "C_CHROMA_FORMAT" "C_SEPARATE_YC_COEFS" "C_SEPARATE_HV_COEFS"
}


#
# Given a list of arguments, define each as a canonical constant name, using
# the driver name, in an include file.
#
proc xdefine_canonical_xpars {drv_handle file_name drv_string args} {
    # Open include file
    set file_handle [::hsm::utils::open_include_file $file_name]

    # Get all the peripherals connected to this driver
    set periphs [::hsm::utils::get_common_driver_ips $drv_handle]

    # Get the names of all the peripherals connected to this driver
    foreach periph $periphs {
        set peripheral_name [string toupper [get_property NAME $periph]]
        lappend peripherals $peripheral_name
    }

    # Get possible canonical names for all the peripherals connected to this driver
    set device_id 0
    foreach periph $periphs {
        set canonical_name [string toupper [format "%s_%s" $drv_string $device_id]]
        lappend canonicals $canonical_name
        
        # Create a list of IDs of the peripherals whose hardware instance name
        # doesn't match the canonical name. These IDs can be used later to
        # generate canonical definitions
        if { [lsearch $peripherals $canonical_name] < 0 } {
            lappend indices $device_id
        }
        incr device_id
    }

    set i 0
    foreach periph $periphs {
        set periph_name [string toupper [get_property NAME $periph]]

        # Generate canonical definitions only for the peripherals whose
        # canonical name is not the same as hardware instance name
        if { [lsearch $canonicals $periph_name] < 0 } {
            puts $file_handle "/* Canonical definitions for peripheral $periph_name */"
            set canonical_name [format "%s_%s" $drv_string [lindex $indices $i]]

            foreach arg $args {
                set lvalue [::hsm::utils::get_driver_param_name $canonical_name $arg]

    # The commented out rvalue is the name of the instance-specific constant
    #           set rvalue [::hsm::utils::get_ip_param_name $periph $arg]

                # The rvalue set below is the actual value of the parameter
                set rvalue [get_property CONFIG.$arg $periph]
                if {[llength $rvalue] == 0} {
                    set rvalue 0
                }
                set rvalue [::hsm::utils::format_addr_string $rvalue $arg]
    
                puts $file_handle "#define $lvalue $rvalue"

            }
            puts $file_handle ""
            incr i
        }
    }

    puts $file_handle "\n/******************************************************************/\n"
    close $file_handle
}