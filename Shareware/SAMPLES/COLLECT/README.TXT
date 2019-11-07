.START:
.AUTHOR: vinooc
.PRODUCT: P_OLE
.TITLE: OLE Automation Collection Sample
.VERSION(S): 3.00 3.10
.OPERATING SYSTEM(S): WINDOWS
.SECURITY: ENDUSER
.REFERENCE: softlib
.ARTICLE TEXT:

Summary:

COLLECT demonstrates how to create an OLE Automation Collection object.

COLLECT can be found in the Software/Data Library by searching on the 
word COLLECT, the Q number of this article, or Sxxxxx. COLLECT was 
archived using the PKware file-compression utility.

More Information:

The collection object implements the following methods and properties:
Add Method, Count Property, Item Property, NewEnum Property and Remove 
Method.

Error handling in the the collection object involves raising exceptions.

Add Method	
----------
Description:	Adds the indicated item to the collection.  If an 
                object is created, it should be returned.
Arguments:	Varies. It can be a pointer to the object that is to 
                be added or it can the information required to create 
                the object. 
Return type:	Varies.  If no object is created, the return type 
                should be void.  If an object is created, the return 
                type should be VT_DISPATCH.
Note:	        The Add method is not appropriate for all collections, 
                so it is not required.  For many application-created 
                collections, objects are automatically added to the 
                collection for the user.

Count Property
--------------
Description:	Returns the number of items in the collection. Read 
                only property.
Arguments:	None. 
Return type:	VT_I4.

Item Property	
-------------
Description:	Returns the indicated item in the collection.
Argument:	Specifies the index. Some collections allow various 
                types of indexing. For example, this sample allows an 
                integer or string to be specified as an index. 
Return type:	VT_DISPATCH.
Note:	        Item is the default value for the object, so it should 
                have the special DISPID, DISPID_VALUE.  MkTypLib 
                automatically assigns this DISPID if the default 
                attribute is specified in the ODL file.

NewEnum Property	
----------------
Description:	Returns an enumerator for that supports IEnumVariant 
                for the items currently in the collection. Read only 
                property.
Arguments:	None. 
Return type:	VT_UNKNOWN.
Note:	        NewEnum will not be accessible to users and must have 
                the restricted attribute in the ODL file. The NewEnum 
                method must have a special DISPID, DISPID_NEWENUM. The 
                defining characteristic of a collection is the ability 
                for a user to iterate over the items in it.  Some 
                languages will have built-in support for collections.  
                The NewEnum method allows an OLE Automation controller 
                to support "for each" iteration over a collection:
	           For Each Item In Collection
		      Debug.Print Item.Text
	           Next Item  
	        OLE Automation controllers that support "for each" 
                iteration will call the NewEnum method on the 
                collection object and then QueryInterface on the 
                resulting IUnknown to get the desired IEnumVariant.

Remove Method	
-------------
Description:	Removes the specified item from the collection.
Argument:	Specifies the index. Some collections allow various 
                types of indexing. For example, this sample allows an 
                integer or string to be specified as an index. 
Return type:	void.
Note:	        The object is not deleted.  It is simply removed from 
                the collection. Remove should support the same kinds 
                of indexing as the Item() method for the same 
                collection.  The Remove method is not appropriate for 
                all collections, so it is not required.  For many 
                application-created collections, objects are 
                automatically removed the collection for the user.

To Run:
-------

The Collection Sample Application object exposes the following:

ProgID :  Collection.Application

Method & Property Names       Notes
-----------------------       ---------
[App Object]
Collection  (read only prop)  Returns empty collection.
New Item    (method)          Creates and returns new item.

[Item Object]
Text        (default prop)    Sets and returns string.

[Collection Object]
See above for exposed properties and methods.

Update the path in hello.reg to the current location of the object and 
the type libraries.

To Compile:
----------
Requires OLE 2.01 or later.

Include device=vmb.386 in the [386Enh] section of system.ini. 
vmb.386 can be found in \OLE2\TOOLS.
Run the wxserver.exe from \OLE2\TOOLS before running the makefile.

KBCategory: le2_ato	
KBSubcategory: le2_ato_id
Additional reference words: OLE2 collection server
\*
\* MSINTERNAL:
\*             Qnumber: n/a
\*             Snumber: n/a <unless modifying an existing file>
\*             Machine: IBM
\*             FileName: COLLECT.ZIP
\*             ReplaceFile: y 
\*             Security: ENDUSER
\*             FileLocation: \\online\submit!WINDEV
\*             SubmittedBy: VINOOC SHARONWA CHARLESN
\*             Product: WINSDK
\*             OperatingEnvironment: WINDOWS
\*             Version: 3.0 3.1
\*             Title: OLE Automation collection sample
\*             Keywords: OLE OLE2 collection automation
\*             OnLineDescription: ----------------50
\*  Demonstrates how to implement an OLE Automation  
\*  collection.
\*
\*=END=
\*
\* Tracker SR#: <SR #>
\* Reviewed by:
\*
.END:
