using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace WebSite.Models
{
    public partial class Value
    {
        public Value()
        {
            value1 = null;
            date = null;
            financial_product_id = null;
            price_portfolio_id = null;
            error_modele_id = null;
            ic = null;
        }

        public Value(double value, double date1, int? financial_product_id1, int? price_portfolio_id1, int? error_modele_id1,double? ic1){
            value1 = value;
            date = date1;
            financial_product_id = financial_product_id1;
            price_portfolio_id = price_portfolio_id1;
            error_modele_id = error_modele_id1;
            ic = ic1;
        }
    }
}