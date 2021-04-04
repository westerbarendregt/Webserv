<?php
/**
 * The base configuration for WordPress
 *
 * The wp-config.php creation script uses this file during the
 * installation. You don't have to use the web site, you can
 * copy this file to "wp-config.php" and fill in the values.
 *
 * This file contains the following configurations:
 *
 * * MySQL settings
 * * Secret keys
 * * Database table prefix
 * * ABSPATH
 *
 * @link https://codex.wordpress.org/Editing_wp-config.php
 *
 * @package WordPress
 */

// ** MySQL settings - You can get this info from your web host ** //
/** The name of the database for WordPress */
define( 'DB_NAME', 'wordpress' );

/** MySQL database username */
define( 'DB_USER', 'admin' );

/** MySQL database password */
define( 'DB_PASSWORD', 'cheese' );

/** MySQL hostname */
define( 'DB_HOST', 'localhost' );

/** Database Charset to use in creating database tables. */
define( 'DB_CHARSET', 'utf8' );

/** The Database Collate type. Don't change this if in doubt. */
define( 'DB_COLLATE', '' );

/**#@+
 * Authentication Unique Keys and Salts.
 *
 * Change these to different unique phrases!
 * You can generate these using the {@link https://api.wordpress.org/secret-key/1.1/salt/ WordPress.org secret-key service}
 * You can change these at any point in time to invalidate all existing cookies. This will force all users to have to log in again.
 *
 * @since 2.6.0
 */
define('AUTH_KEY',         '-NBr7*A{3DLH+%`No>vLMR?OI&K4;D4N>*/l)rtT:yeO`l:iIPiY+qbg1k5ztR$y');
define('SECURE_AUTH_KEY',  'xy- w2^pu|#|)e)+Va9e9Q0~3nhS%)<p;k?[b!b;[@I&ZUs&BeU=9L|u*A?AiP`z');
define('LOGGED_IN_KEY',    'aLR}a||<)K<KHwA4Ab.p2q?n9PT+N]h.Y:7xg[0J8$R.,B[vK=g04.U=-clQb}80');
define('NONCE_KEY',        'kpk=uT!E;nubJc:fdVvDTGLD7cL:zp%xj_b|`&x1.K%P {-pK9l6Tp9BSpEi}nmp');
define('AUTH_SALT',        'TAx,(mm5)|oDL%ys%5C8EMyVE3L-C[<4q<y%0;_&||;>uV$!z1egRKwZ(huI?R@W');
define('SECURE_AUTH_SALT', 'zzO{0_@u4ie2}{2qn$lu,<w[WrE$i4GHd/V-i`tbMq4&.oMSk+t;x8u4|{L0.omb');
define('LOGGED_IN_SALT',   'SOO<Kq?m%hK=cX}sMm|GsEV`uAz:&bRR%geiT)L-Hj^*ESHO:h{u^8vlz=$i/QiC');
define('NONCE_SALT',       'o6q*ooVY-PLQvH-mu*|eC5Jm7+re8w jU6<([FR+<2y&w, >9sn:n,6mXgN^ n(}');

/**#@-*/

/**
 * WordPress Database Table prefix.
 *
 * You can have multiple installations in one database if you give each
 * a unique prefix. Only numbers, letters, and underscores please!
 */
$table_prefix = 'wp_';

/**
 * For developers: WordPress debugging mode.
 *
 * Change this to true to enable the display of notices during development.
 * It is strongly recommended that plugin and theme developers use WP_DEBUG
 * in their development environments.
 *
 * For information on other constants that can be used for debugging,
*/

define( 'WP_DEBUG', false );

/* That's all, stop editing! Happy publishing. */

/** Absolute path to the WordPress directory. */
if ( ! defined( 'ABSPATH' ) ) {
	define( 'ABSPATH', dirname( __FILE__ ) . '/' );
}

/** Sets up WordPress vars and included files. */
require_once( ABSPATH . 'wp-settings.php' );