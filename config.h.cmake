/* begin ewoms-eclgrids
   put the definitions for config.h specific to
   your project here. Everything above will be
   overwritten
*/
/* begin private */
/* Name of package */
#define PACKAGE "@DUNE_MOD_NAME@"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "@DUNE_MAINTAINER@"

/* Define to the full name of this package. */
#define PACKAGE_NAME "@DUNE_MOD_NAME@"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "@DUNE_MOD_NAME@ @DUNE_MOD_VERSION@"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "@DUNE_MOD_NAME@"

/* Define to the home page for this package. */
#define PACKAGE_URL "@DUNE_MOD_URL@"

/* Define to the version of this package. */
#define PACKAGE_VERSION "@DUNE_MOD_VERSION@"

/* Hack around some ugly code in the unit tests. */
#define HAVE_DYNAMIC_BOOST_TEST 1
#define BOOST_TEST_DYN_LINK 1

/* end private */

/* Define to the version of ewoms-eclgrids */
#define EWOMS_ECLGRIDS_VERSION "${EWOMS_ECLGRIDS_VERSION}"

/* Define to the major version of ewoms-eclgrids */
#define EWOMS_ECLGRIDS_VERSION_MAJOR ${EWOMS_ECLGRIDS_VERSION_MAJOR}

/* Define to the minor version of ewoms-eclgrids */
#define EWOMS_ECLGRIDS_VERSION_MINOR ${EWOMS_ECLGRIDS_VERSION_MINOR}

/* Define to the revision of ewoms-eclgrids */
#define EWOMS_ECLGRIDS_VERSION_REVISION ${EWOMS_ECLGRIDS_VERSION_REVISION}

/* Specify whether the ZOLTAN graph partitioning library is available or not */
#cmakedefine HAVE_ZOLTAN 1

/* ignore a pesky deprecation warning for dune-grid >= 2.5 */
#define DUNE_AVOID_CAPABILITIES_IS_PARALLEL_DEPRECATION_WARNING 1

/* begin bottom */

/* end bottom */

/* end ewoms-eclgrids */
