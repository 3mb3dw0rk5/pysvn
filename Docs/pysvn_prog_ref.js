//
// ====================================================================
// Copyright (c) 2006 Barry A Scott.  All rights reserved.
//
// This software is licensed as described in the file LICENSE.txt,
// which you should have received as part of this distribution.
//
// ====================================================================
//
all_versions = new Array( 1001000, 1002000, 1003000 )

change_style = "background"

function changeDisplay( version )
{
    style_string = ""

    if( change_style == "background" )
    {
        span_style = "";
        div_style = "";
    }
    else
    {
        span_style = "display: inline";
        div_style = "display: block";
    }

    excluded_features = false;

    for( var i=0; i<all_versions.length; i++)
    {
        if( version < all_versions[i] )
        {
            if( change_style == "background" )
            {
                span_style = "background-color: #ffc0c0;";
                div_style = "background-color: #ffc0c0;";
            }
            else
            {
                span_style = "display: none";
                div_style = "display: none";
            }
        }

        style_string += "span.svn_" + all_versions[i].toString() + " { " + span_style + ";}\n";
        style_string += "div.svn_" + all_versions[i].toString() + " { " + div_style + ";}\n";
    }

    document.getElementById("example_show_style").innerHTML = "span.example_show_style { " + span_style + ";}\n";;

    document.getElementById("show_style").innerHTML = style_string;

    major = Math.floor( version / 1000000 ); 
    minor = Math.floor( version / 1000 ) % 1000;
    patch = version % 1000;
    version_shown = "" + major + "." + minor + "." + patch;

    document.getElementById("version_shown").innerHTML = version_shown;
}
