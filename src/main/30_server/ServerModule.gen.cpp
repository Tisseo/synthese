
synthese::util::Factory<synthese::util::ModuleClass>::integrate<synthese::server::ServerModule>("999_server");


synthese::util::Factory<synthese::server::Action>::integrate<synthese::server::LoginAction>("login");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::server::LogoutAction>("logout");

synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::server::HtmlFormInterfaceElement>("htmlform");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::server::LogoutHTMLLinkInterfaceElement>("logouthtmllink");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::server::LoginHtmlField>("loginhtmlfield");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::server::PasswordHtmlField>("passwordhtmlfield");
synthese::util::Factory<synthese::interfaces::ValueInterfaceElement>::integrate<synthese::server::RequestErrorMessageInterfaceElement>("errormessage");
