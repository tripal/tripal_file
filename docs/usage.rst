Usage
=====

Adding a License
----------------
To ensure data files offered by your Tripal site meet `FAIR data principles <https://www.go-fair.org/fair-principles/>`_, all files must be associated with a license.  Therefore, before adding files, you must first create new **License** content types.  You can create as many **License** files as needed for the data on your site.

To create a new license page, navigate to **Admin** > **Add Tripal Content Types** and scroll to the bottom of the page where the **Other** category is found.  There you will see the **License** content type.  Click the link to create a new license page.

.. image:: ./license_page1.png

To create your license page you can:

1. Option 1: Fully define the license by giving it a name and provide the full description
2. Option 2: Summarize the license by giving a name, providing a brief summary (or no summary) and providing the URL to the full license online.

.. note::

    It is best practice to provide a human readable summary of the user's rights in the Summary field and to provide a link to the full legal text of the license via the URI field.

Example Licenses
````````````````
A few example licenses are provided below along with the values that may be appropriate for the **License** page fields.

Public Domain Data
~~~~~~~~~~~~~~~~~~
Data that is public, where no on retains rights to it. Therefore, it needs a license page even though "public domain" is not an actual license. You can use the following values when creating a public domain license page:

+---------+-------------------------------------------------------------------+
| Field   | Value                                                             |
+=========+===================================================================+
| Name    | Public Domain                                                     |
+---------+-------------------------------------------------------------------+
| Summary | The data has no known copyrights or restrictions and              |
|         | can be be freely used by anyone.                                  |
+---------+-------------------------------------------------------------------+

CC0 1.0 Universal (CC0 1.0)
~~~~~~~~~~~~~~~~~~~~~~~~~~~
The text for the summary below came from https://creativecommons.org/publicdomain/zero/1.0/.

+---------+-------------------------------------------------------------------+
| Field   | Value                                                             |
+=========+===================================================================+
| Name    | CC0 1.0 Universal (CC0 1.0)                                       |
+---------+-------------------------------------------------------------------+
| Summary | The person who associated a work with this deed has dedicated the |
|         | work to the public domain by waiving all of his or her rights to  |
|         | the work worldwide under copyright law, including all related and |
|         | neighboring rights, to the extent allowed by law. You can copy,   |
|         | modify, distribute and perform the work, even for                 |
|         | commercial purposes, all without asking permission.               |
|         |                                                                   |
|         | - In no way are the patent or trademark rights of any person      |
|         |   affected by CC0, nor are the rights that other persons may have |
|         |   in the work or in how the work is used, such as publicity or    |
|         |   privacy rights.                                                 |
|         | - Unless expressly stated otherwise, the person who associated a  |
|         |   work with this deed makes no warranties about the work, and     |
|         |   disclaims liability for all uses of the work, to the fullest    |
|         |   extent permitted by applicable law.                             |
|         | - When using or citing the work, you should not imply endorsement |
|         |   by the author or the affirmer.                                  |
+---------+-------------------------------------------------------------------+
| URI     | https://creativecommons.org/publicdomain/zero/1.0/legalcode       |
+---------+-------------------------------------------------------------------+

Attribution 4.0 International (CC BY 4.0)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The text for the summary below came from https://creativecommons.org/licenses/by/4.0/.

+---------+-------------------------------------------------------------------+
| Field   | Value                                                             |
+=========+===================================================================+
| Name    | Attribution 4.0 International (CC BY 4.0)                         |
+---------+-------------------------------------------------------------------+
| Summary | You are free to:                                                  |
|         |                                                                   |
|         | - **Share**: copy and redistribute the material in any medium     |
|         |   or format                                                       |
|         | - **Adapt**: remix, transform and build upon the material for any |
|         |   purpose, even commercially.                                     |
|         |                                                                   |
|         | The licensor cannot revoke these freedoms as long as you follow   |
|         | the following license terms:                                      |
|         |                                                                   |
|         | - **Attribution** You must give appropriate credit, provide a     |
|         |   link to the license, and indicate if changes were made. You may |
|         |   do so in any reasonable manner, but not in any way that         |
|         |   suggests the licensor endorses you or your use.                 |
|         | - **No additional restrictions** You may not apply legal terms or |
|         |   technological measures that legally restrict others from doing  |
|         |   anything the license permits                                    |
|         |                                                                   |
|         | **Notices:**                                                      |
|         |                                                                   |
|         | - You do not have to comply with the license for elements of the  |
|         |   material in the public domain or where your use is permitted by |
|         |   an applicable exception or limitation.                          |
|         | - No warranties are given. The license may not give you all of    |
|         |   the permissions necessary for your intended use. For example,   |
|         |   other rights such as publicity, privacy, or moral rights may    |
|         |   limit how you use the material.                                 |
+---------+-------------------------------------------------------------------+
| URI     | https://creativecommons.org/licenses/by/4.0/legalcode             |
+---------+-------------------------------------------------------------------+

Adding a File
-------------
To demonstrate adding a file, we will provide an example using the genome files that are provided in the `Tripal User's Guide <https://tripal.readthedocs.io/en/latest/index.html>`_ in the section titled `Setup of an Example Site <https://tripal.readthedocs.io/en/latest/user_guide/example_genomics.html>`_. Those instructions direct the reader to create an **Analysis** page to represent the whole genome assembly of the *Citrus sinensis* v1.0 genome. We will associate the files to that analysis.  If you have followed along with the Tripal User's Guide you will already have those files uploaded.

If you have not followed the Tripal User's Guide but would like to follow along here, you must follow `these instructions to create an Analysis page <https://tripal.readthedocs.io/en/latest/user_guide/example_genomics/analyses.html>`_. And obtain the files here:

- `Citrus sinensis-orange1.1g015632m.g.gff3 <http://tripal.info/sites/default/files/Citrus_sinensis-orange1.1g015632m.g.gff3>`_
- `Citrus sinensis-scaffold00001.fasta <http://tripal.info/sites/default/files/Citrus_sinensis-scaffold00001.fasta>`_
- `Citrus sinensis-orange1.1g015632m.g.fasta <http://tripal.info/sites/default/files/Citrus_sinensis-orange1.1g015632m.g.fasta>`_

To create a new file page, navigate to **Admin** > **Add Tripal Content Types** and scroll to the bottom of the page where the **Other** category is found.  There you will see the **File** content type.  Click the link to create a new file page. The following page appears.

.. image:: ./file_add1.png

On this page you caution

1. Provide a **name** (required) and **description** for the file.
2. Indicate a file **type** (required).
3. Indicate the file **source** (or contact person) who was rights to the data.
4. Indicate the file **location** (either locally or via remote URL).
5. And set the **license** (required).

First, let's upload the whole genome FASTA file.  Enter the following in the fields:

+--------------+--------------------------------------------------------------+
| Field        | Value                                                        |
+==============+==============================================================+
| Name         | *Citrus sinesis* Whole Genome Assembly v1.0                  |
+--------------+--------------------------------------------------------------+
| Description  | The whole genome assembly, v1.0, of *Citrus sinensis*.       |
+--------------+--------------------------------------------------------------+
| Type         | FASTA                                                        |
+--------------+--------------------------------------------------------------+
| File Source  | *Leave blank or provide any contact you may have already*    |
+--------------+--------------------------------------------------------------+
| Location     | Citrus sinensis-scaffold00001.fasta                          |
+--------------+--------------------------------------------------------------+
| File License | *Select whatever license you may have already added*         |
+--------------+--------------------------------------------------------------+

After creation a file page appears:

.. image:: ./file_page1.png
