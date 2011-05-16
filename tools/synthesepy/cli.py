#    synthesepy command line interface.
#    @file cli.py
#    @author Sylvain Pasche
#
#    This file belongs to the SYNTHESE project (public transportation specialized software)
#    Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.


import argparse
import logging
import time

import synthesepy
import synthesepy.build
import synthesepy.daemon
import synthesepy.db_backends
import synthesepy.env
import synthesepy.sqlite_to_mysql
import synthesepy.test.main
import synthesepy.utils

log = logging.getLogger(__name__)


def build(args, env):
    synthesepy.build.build(env, args)


def rundaemon(args, env):
    daemon = synthesepy.daemon.Daemon(env)
    daemon.start()
    # TODO: show URL in info message.
    log.info('Daemon running, press ctrl-c to stop')
    try:
        while True:
            time.sleep(10)
    except KeyboardInterrupt:
        print '^C'

    log.info('Stopping daemon')
    daemon.stop()


def stopdaemon(args, env):
    # TODO: should use the HTTP method to stop the daemon once it works.
    synthesepy.utils.kill_listening_processes(env.port)


def runtests(args, env):
    tester = synthesepy.test.main.Tester(env, args)
    tester.run_tests(args.suites)


def initdb(args, env):
    backend = synthesepy.db_backends.create_backend(env, args.conn_string)
    backend.init_db()


def sqlite_to_mysql(args, env):
    synthesepy.sqlite_to_mysql.convert(env, args.sourceconn, args.targetconn)


def main():
    parser = argparse.ArgumentParser(description='Synthese management tool')
    parser.add_argument('-t', '--env-type', choices=['scons', 'cmake', 'installed'])
    parser.add_argument('-b', '--env-path', help='Env path')
    parser.add_argument('-m', '--mode', choices=['release', 'debug'])
    parser.add_argument('-p', '--port', type=int, default=8080)
    parser.add_argument('-d', '--dbconn', default='sqlite://', dest='conn_string')
    parser.add_argument('-v', '--verbose', action='store_true', default=False)
    parser.add_argument(
        '-s', '--stdout', action='store_true', default=False, dest='log_stdout',
        help='Log daemon output to stdout'
    )
    parser.add_argument(
        '--static-dir',
        help='Directory containing static files served by the HTTP proxy'
    )
    parser.add_argument(
        '-e', '--extra-params',
        help='Daemon extra parameters, using format "param0=value0 param1=value1"'
    )

    subparsers = parser.add_subparsers(help='sub-command help')

    parser_build = subparsers.add_parser('build')
    parser_build.set_defaults(func=build)
    parser_build.add_argument(
        '--with-mysql', action='store_true', default=False,
        help='Enable MySQL database support'
    )
    parser_build.add_argument(
        '--mysql-dir',
        help='Path to MySQL installation (Not needed on Linux if using'
             'standard MySQL installation)'
    )

    parser_rundaemon = subparsers.add_parser('rundaemon')
    parser_rundaemon.set_defaults(func=rundaemon)

    parser_stopdaemon = subparsers.add_parser('stopdaemon')
    parser_stopdaemon.set_defaults(func=stopdaemon)

    parser_runtests = subparsers.add_parser('runtests')
    parser_runtests.set_defaults(func=runtests)
    # for python suite
    parser_runtests.add_argument(
        '--dbconns', nargs='+', dest='conn_strings', default=[]
    )
    # for python suite
    parser_runtests.add_argument(
        '--no-init',
        help='Don\'t start/stop the daemon or initialize the db. '
             'Can be used to reuse an already running daemon',
        action='store_true', default=False
    )
    parser_runtests.add_argument(
        'suites', nargs='*',
        help='List of test suites to run. Choices: style, python, cpp'
    )

    parser_initdb = subparsers.add_parser('initdb')
    parser_initdb.set_defaults(func=initdb)

    parser_sqlite_to_mysql = subparsers.add_parser('sqlite_to_mysql')
    parser_sqlite_to_mysql.set_defaults(func=sqlite_to_mysql)
    parser_sqlite_to_mysql.add_argument('--sourceconn')
    parser_sqlite_to_mysql.add_argument('--targetconn')

    args = parser.parse_args()

    logging.basicConfig(level=(logging.DEBUG if args.verbose else logging.INFO))

    env = synthesepy.env.create_env(args.env_type, args.env_path, args.mode)
    env.port = args.port
    env.wsgi_proxy_port = env.port + 1
    env.conn_string = args.conn_string
    env.log_stdout = args.log_stdout
    env.static_dir = args.static_dir
    env.extra_params = args.extra_params

    log.debug('Args: %s', args)
    args.func(args, env)

if __name__ == '__main__':
    main()
